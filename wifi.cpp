#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
using namespace std;

string extractSignalLevel(const string& iwlistOutput) {
    size_t signalLevelPos = iwlistOutput.find("Signal level=");
    if (signalLevelPos == string::npos) {
        return "Signal level not found";
    }

    size_t start = iwlistOutput.find('=', signalLevelPos) + 1;
    size_t end = iwlistOutput.find(' ', start);

    if (start == string::npos || end == string::npos) {
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

        execl("/bin/sh", "sh", "-c", "sudo iwlist wlan0 scan | grep -A 5 -B 5 'ESSID:\"csu-net\"' | grep 'Signal level'", (char*) NULL);
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
        string essid = "csu-net"; // Replace with your ESSID
        string signalLevel = extractSignalLevel(output);
        cout << "Signal Level for " << essid << ": " << signalLevel << endl;

        return signalLevel;
    }

}