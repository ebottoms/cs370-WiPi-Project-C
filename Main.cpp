#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <thread>
#include <pthread.h>
#include <chrono>
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
    string logName = "log0.csv";
    while (logExists(logName.c_str()) && suffix < 1000)
    {
        suffix += 1;
        logName = "log" + to_string(suffix) + ".csv";
    }

    ofstream logFile;
    logFile.open("logs/"+logName);

    while (isRecording)
    {
        string time = "Time";
        string lattitude = "Lat";
        string longitude = "Long";
        string signalStrength = "sigStr";
        logFile << time + "," + lattitude + "," + longitude + "," + signalStrength + "\n";
        this_thread::sleep_for(chrono::milliseconds(5000));
    }
    
    logFile.close();
}

int main(int argc, char const *argv[])
{
    isRecording = true;
    thread recorder(run);
    do {
        cout << '\n' << "Press ENTER to stop recording...";
    } while (cin.get() != '\n');
    isRecording = false;
    recorder.join();
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