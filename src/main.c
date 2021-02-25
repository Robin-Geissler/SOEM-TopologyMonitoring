#include <stdio.h>
#include <string.h>
#include <time.h>

#include "promodularSOEM.h"

int main(int argc, char *argv[]) {
    /** Buffer to save the name of the Network Interface*/
    char netInterface[1024];
    /** Buffer to map all IO from slaves*/
    char iOmap[4096];
    /** Context to save bus information*/
    ecx_contextt *context;
    /** Current working counter used to detect topology changes*/
    int wkc = 0;

    /** Clocks for time measurments*/
    struct timespec t1;
    struct timespec t2;



    long secdif;
    long nanodif;
    long millidif;
    long nanodif2;
    long microdif;
    long nanodif3;

    printf("Starting PromodularSOEM ... \n");

    if(argc < 2){
        printf("No network interface specified\n");
        return -1;
    }
    strncpy(netInterface,argv[1],1024);

    /* Init the EC Master*/
    printf("EC init\n");
    if(!ec_init(netInterface)){
        printf("No bus connection, make sure to run as super user\n");
        return -1;
    }
    printf("Init on %s succeeded.\n", netInterface);

    /* Scan and init the Bus*/
    context = busMemberScan(iOmap, &wkc);
    /* generate first Visualization*/
    visualizeTopology(context);

    /********************Measurement 1: Cycle Time*********************************************************************/
//    /** Read Buffer 16 Bit for cycle time measurment*/
//    uint16 r16;
//    FILE * file;
//    file = fopen("../../measurements/cycleTimes.csv","w");
//    fprintf(file, "Cycle Time\n");
//
//    for(int i = 0; i < 10000; i++) {
//        clock_gettime(CLOCK_MONOTONIC, &t1);
//        ecx_BRD(context->port, 0x0000, ECT_REG_TYPE, sizeof(r16), &r16, EC_TIMEOUTSAFE);
//        clock_gettime(CLOCK_MONOTONIC, &t2);
//        if (t2.tv_nsec - t1.tv_nsec < 0) {
//            secdif = t2.tv_sec - t1.tv_sec - 1;
//            nanodif = t2.tv_nsec - t1.tv_nsec + 1000000000;
//        } else {
//            secdif = t2.tv_sec - t1.tv_sec;
//            nanodif = t2.tv_nsec - t1.tv_nsec;
//        }
//        fprintf(file, "%ld\n", nanodif);
//    }
//    fclose(file);
//    printf("finished with measurements\n");
/**********************************************************************************************************************/


    FILE * file;
    file = fopen("../../measurements/TopologyChangeResponseTimes.csv","w");
    fprintf(file, "SlaveConfigNumber,Response Time\n");
    int i = 0;
    while(i < 5){



//        printf("WKCDetected: %d\n", wkcDetected);
//        printf("WKC: %d\n", wkc);
//        printf("Write was: %d\n", r16);



        clock_gettime(CLOCK_MONOTONIC, &t1);
        /* If Topology change is detected scan, re-init bus, visualize new Topology*/
        if(detectTopologyChange(wkc, context)){

            /* some slaves like EK1100 need some time to config there slaves, if you read them out to early the EEprom
             * will not be working*/
            /*Über Time stamp lösen -> kein blocking wait mit usleep*/
            /* Bus aus Operational mode nehemen*/
            usleep(SLAVE_CONFIG_TIME_us);
            busMemberScan(iOmap, &wkc);
            visualizeTopology(context);
           /******************************Measure Topology response Time***********************************************/
            clock_gettime(CLOCK_MONOTONIC, &t2);

            if(t2.tv_nsec - t1.tv_nsec < 0){
                secdif = t2.tv_sec - t1.tv_sec - 1;
                nanodif = t2.tv_nsec - t1.tv_nsec + 1000000000;
            }else{
                secdif = t2.tv_sec - t1.tv_sec;
                nanodif = t2.tv_nsec - t1.tv_nsec;
            }

            // Split nanos in millis, micros and nanos
            millidif = nanodif / 1000000;
            nanodif2 = nanodif % 1000000;
            microdif = nanodif2 / 1000;
            nanodif3 = nanodif2 % 1000;

            printf("Seconds: %ld\nNanos: %ld\nMillis: %ld\nNikros %ld\nNanos %ld\n\n", secdif, nanodif, millidif, microdif, nanodif3);
            fprintf(file,"%d,%ld\n",wkc,nanodif);
            i++;
            printf("%d\n", i);
            /**********************************************************************************************************/
        }
    }

    fclose(file);

    return 0;
}



