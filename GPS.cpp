#include <libgpsmm.h>
#include <iostream>
#include <istream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <bits/timex.h>
#include <gps.h>
#include <unistd.h>
using namespace std;



void print(gps_data_t* newdata) {
    
    printf("Lat: %.7lf\n", newdata->fix.latitude);
    printf("Lon: %.7lf\n", newdata->fix.longitude);
    printf("Time (tv_sec): %.ld \n", newdata->fix.time.tv_sec);
    printf("_________\n");
    return;
}

gps_data_t* gpsRead(gpsmm gps_rec){ // deprecated
    struct gps_data_t* data;
    if ((data = gps_rec.read()) == NULL) {
        printf("Error Reading From GPSD.\\n");
    } else {
        // Check if a valid fix is available
        if (data->fix.mode >= MODE_2D) {
            printf("Successful Capture of GPSD!\\n");
            print(data);
        } else {
            printf("GPSD data not ready.\\n");
        }
    }
    return data;

}

void write(gps_data_t* data, void* shmptr){
    double* shmdata = static_cast<double*>(shmptr);
    shmdata[0] = data->fix.latitude;
    shmdata[1] = data->fix.longitude;
    return;

}

int main(void) {
    gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);


    key_t key = 1111;
    int shmid = shmget(key, (2 * sizeof(string)), 0666 | IPC_CREAT);
    void* shmptr = shmat(shmid, nullptr, 0);

    if(shmptr == (void*)-1){
        printf("GPS: Failed Connecting to Shared Memory!");
        return -1;
    }

    while (gps_rec.stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
        printf("No GPSD running.\n");
        gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);
    }

    gps_data_t* data;
    

    for (;;) {
        struct gps_data_t* data;

        if (!gps_rec.waiting(50000000))
        continue;

        if ((data = gps_rec.read()) == NULL) {
            printf("Error Reading From GPSD.\n");
            return 1;
        } else {
            printf("Successful Capture of GPSD!\n");
            print(data);
            write(data, shmptr);

            printf("GPS: Wrote to Shared Memory.");

        }
    }
    

    return 0;

    
}

