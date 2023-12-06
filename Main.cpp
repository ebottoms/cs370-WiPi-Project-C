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

void gpsThread(){
    while (isRecording) {
        system("./GPS");
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    

}

void run() {
    int suffix = 0;
    string logName = "log0.csv";
    while (logExists(logName.c_str()) && suffix < 1000)
    {
        suffix += 1;
        logName = "log" + to_string(suffix) + ".csv";
    }
    ofstream logFile;
    logFile.open("logs/"+logName);
    // child process for GPS, set GPS to write read pipe from gps
    
    //thread gpsThreadHandle(gpsThread);
    
    this_thread::sleep_for(chrono::milliseconds(120000));

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
        

        // read from gps pipe or shared memory
        // run wifi scan method
        
        string time = "Time";
        string lattitude = to_string(shmdata[0]);
        string longitude = to_string(shmdata[1]);
        string signalStrength = readWiFi();
        logFile << time + "," + lattitude + "," + longitude + "," + signalStrength + "\n";
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    logFile.close();
    
    //gpsThreadHandle.join();

}

int main(int argc, char const *argv[])
{
    isRecording = true;
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        execlp("./GPS", "GPS", (char *) NULL);

        // execlp only returns if there's an error
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        // ... your code ...
        thread recorder(run);
        do {
            cout << '\n' << "Press ENTER to stop recording...";
        } while (cin.get() != '\n');
        isRecording = false;
        recorder.join();

        // When you need to stop the command
        kill(pid, SIGKILL);

        // Wait for child to terminate
        waitpid(pid, NULL, 0);
    }
    /*
    bool restart = false;
    while (true) {
        cout << '\n' << "Type 'R' to record again or anything else to exit.";
        char input = cin.get();
        if(cin.get() != 'R' || cin.get() != 'r') {
            restart = true;
            break;
        } else if (cin.get() != '\n') {
            break;
        }
    }
    */
    
    return 0;
}