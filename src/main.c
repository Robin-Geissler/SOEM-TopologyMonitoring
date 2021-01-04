#include <stdio.h>
#include <string.h>

#include "promodularSOEM.h"

int main(int argc, char *argv[]) {

    char netInterface[1024];
    char iOmap[4096];
    ecx_contextt *context;
    int wkc = 0;
    int wkcDetected = 0;
    uint16 r16;

    printf("Starting PromodularSOEM ... \n");

    if(argc < 2){
        printf("No network interface specified\n");
        return -1;
    }
    strncpy(netInterface,argv[1],1024);


    printf("EC init\n");
    if(!ec_init(netInterface)){
        printf("No bus connection, make sure to run as super user\n");
        return -1;
    }
    printf("Init on %s suceeded.\n", netInterface);

    /* Scan and init the Bus*/
    context = busMemberScan(iOmap, &wkc);

    while(TRUE){



//        printf("WKCDetected: %d\n", wkcDetected);
//        printf("WKC: %d\n", wkc);
//        printf("Write was: %d\n", r16);


        /* If Topology change is detected scan, re-init bus, visualize new Topology*/
        wkcDetected = ecx_BRD(context->port, 0x0000, ECT_REG_TYPE, sizeof(r16), &r16, EC_TIMEOUTSAFE);  /* detect number of slaves */
        if(wkc != wkcDetected){
            busMemberScan(iOmap, &wkc);
            visualizeTopology(context);
        }

    }



    return 0;
}



