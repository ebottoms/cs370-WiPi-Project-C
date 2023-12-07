#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
using namespace std;

// Extracts signal level from shell output
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

// Reads specified ESSID signal level
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

    // Child process
    if (pid == 0) { 
        // Close read end
        close(pipefd[0]); 
        // Directing stdout to pipe
        dup2(pipefd[1], STDOUT_FILENO); 
        // Close write end
        close(pipefd[1]); 

        // iwlist 
        execl("/bin/sh", "sh", "-c", "sudo iwlist wlan1 scan | grep -A 5 -B 5 'ESSID:\"csu-net\"' | grep 'Signal level'", (char*) NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pipefd[1]); // Close write end

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