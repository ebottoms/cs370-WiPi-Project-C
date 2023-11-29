#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
using namespace std;

string extractSignalLevel(const string& iwlistOutput, const string& essid) {
    std::string searchString = "ESSID:\"" + essid + "\"";
    size_t essidPos = iwlistOutput.find(searchString);
    if (essidPos == string::npos) {
        return "ESSID not found";
    }
    size_t nextEssidPos = iwlistOutput.find("ESSID:", essidPos + searchString.length());
    size_t endOfBlock = nextEssidPos != string::npos ? nextEssidPos : iwlistOutput.length();

    size_t signalLevelPos = iwlistOutput.find("Signal level=", essidPos);
    if (signalLevelPos == string::npos || signalLevelPos > endOfBlock) {
        return "Signal level not found";
    }

    size_t start = iwlistOutput.find('=', signalLevelPos) + 1;
    size_t end = iwlistOutput.find(' ', start);

    if (start == string::npos || end == string::npos || end > endOfBlock) {
        return "Signal level format error";
    }

    return iwlistOutput.substr(start, end - start);
}

string readWiFi() {
    int pipefd[2];
    pid_t pid;
    char buf;
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]); // Close write end of pipe

        execl("/sbin/iwlist", "wlp4s0", "scanning", (char*) NULL);
        perror("execl"); // execl only returns on error
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pipefd[1]); // Close unused write end

        string output;
        while (read(pipefd[0], &buf, 1) > 0) {
            output += buf;
        }

        close(pipefd[0]); // Close read end

        // Wait for child to finish
        wait(NULL);

        // Search for a specific string in the output
        string essid = "Pop Hause"; // Replace with your ESSID
        string signalLevel = extractSignalLevel(output, essid);
        cout << "Signal Level for " << essid << ": " << signalLevel << endl;

        return signalLevel;
    }

}