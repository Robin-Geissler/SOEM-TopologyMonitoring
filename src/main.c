#include <stdio.h>
#include <string.h>

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

    while(TRUE){



//        printf("WKCDetected: %d\n", wkcDetected);
//        printf("WKC: %d\n", wkc);
//        printf("Write was: %d\n", r16);


        /* If Topology change is detected scan, re-init bus, visualize new Topology*/
        if(detectTopologyChange(wkc, context)){
            /* some slaves like EK1100 need some time to config there slaves, if you read them out to early the EEprom
             * will not be working*/
            /*Über Time stamp lösen -> kein blocking wait mit usleep*/
            /* Bus aus Operational mode nehemen*/
            usleep(SLAVE_CONFIG_TIME_us);
            busMemberScan(iOmap, &wkc);
            visualizeTopology(context);
        }

    }



    return 0;
}



