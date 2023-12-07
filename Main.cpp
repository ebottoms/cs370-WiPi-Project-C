#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <thread>
#include <pthread.h>
#include <chrono>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "wifi.h"
using namespace std;

bool isRecording;


bool logExists(string logName) {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir("logs")) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            string entryName = entry->d_name;
            if (strcmp(logName.c_str(), entryName.c_str()) == 0) { 
                return true;
            }
        }
    }

    return false;
}


void run() {
    int suffix = 0;
    // Opens csv log file
    string logName = "log0.csv";
    while (logExists(logName.c_str()) && suffix < 1000)
    {
        suffix += 1;
        logName = "log" + to_string(suffix) + ".csv";
    }
    ofstream logFile;
    logFile.open("logs/"+logName);
    
    this_thread::sleep_for(chrono::milliseconds(120000));

    // Connects to shared memory
    key_t key = 1111;
    int shmid = shmget(key, (2 * sizeof(double)), 0666 | IPC_CREAT);
    void* shmptr = shmat(shmid, nullptr, 0);

    if(shmptr == (void*)-1){
        printf("Main: Failed Connecting to Shared Memory.");
        return;
    }

    while (isRecording)
    {
        double* shmdata = static_cast<double*>(shmptr);
        
        // Run wifi scan method
        string signalStrength = readWiFi();
        // Read from gps shared memory
        string lattitude = to_string(shmdata[0]);
        string longitude = to_string(shmdata[1]);
        logFile << lattitude + "," + longitude + "," + signalStrength + "\n";
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    
    logFile.close();
    
    

}

int main(int argc, char const *argv[])
{
    isRecording = true;
    pid_t pid = fork();

    if (pid == 0) {
        // Child process running GPS process
        execlp("./GPS", "GPS", (char *) NULL);
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process running recording thread and I/O
        thread recorder(run);
        do {
            cout << '\n' << "Press ENTER to stop recording...";
        } while (cin.get() != '\n');
        isRecording = false;
        recorder.join();

        // Stops process 
        kill(pid, SIGKILL);

        // Waiting child termination
        waitpid(pid, NULL, 0);
    }
    
    
    return 0;
}