#include <stdio.h>
#include <string.h>

#include "promodularSOEM.h"

int main(int argc, char *argv[]) {

    char netInterface[1024];
    char iOmap[4096];
    ecx_contextt *context;

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

    while(TRUE){
        context = busMemberScan(iOmap);

        /* Visualize the Topology*/
        visualizeTopology(context);
    }



    return 0;
}



