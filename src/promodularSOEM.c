//
// Created by robin on 23.11.20.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "promodularSOEM.h"


/** Arguments for dot program
 * Program Name
 * Option:
 * Path to the .gv file
 * Option
 * Path to the newly created .png file*/
char *dotArgv[] = {"dot", "-Tpng", "../../vizFiles/graphViz.gv", "-o", "../../vizFiles/graph.png", (char*)0};

ecx_contextt *busMemberScan(char ioMap[], int* wkc){
//    printf("Scannig bus topology...\n");

    /* configure all slaves */
    *wkc = ec_config(FALSE,ioMap);
    if(*wkc < 1){
        printf("No slaves found\n");
        /*exit here*/
    }

//    printf("All slaves configured\n");

    /* Configure distributed clocks*/
    ec_configdc();

    /* wait for all slaves to reach SAFE_OP state*/ /*Frage: Wann geht der Master(slave 0) in EC_STATE_SAFE_OP*/
    ec_statecheck(0,EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 3);
    /* SAFE_OP state not reached*/
    if(ec_slave[0].state != EC_STATE_SAFE_OP){
        printf("Not all slaves reached safe oprational state\n");
        ec_readstate();
        for(int i = 1; i <=ec_slavecount; i++){
            if(ec_slave[i].state != EC_STATE_SAFE_OP){
                printf("Slave %d State=%2x StatusCode=%4x : %s\n",i, ec_slave[i].state,ec_slave[i].ALstatuscode,
                       ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
            }
        }
    }

    ec_readstate();
//    printf("Finished reading out network configuration\n");
    return &ecx_context;
}

/* to get a png image do:
 * dot -Tpng graphViz.gv -o graph.png
 * */
int visualizeTopology(ecx_contextt *ec_context){
    // Visualization happens in new Thread to not delay main process
    if(fork() == 0){

        FILE * fp;

        fp = fopen("../../vizFiles/graphViz.gv","w");

        fprintf(fp, "digraph G {\n\n");

        fprintf(fp, "node_0 [label=\"Master\"]\n");

        for(int i = 1; i <= *(ec_context->slavecount); i++){
//            if(i == 3 || i == 1|| TRUE) {
//                printf("Slave %d\n", i);
//                printf("Name: %s\n", ec_context->slavelist[i].name);
                //           printf("Vendor ID: %x\n", (int) ec_context->slavelist[i].eep_man);
//            printf("Product Code: %d\n", (int) ec_context->slavelist[i].eep_id);
//            printf("Revision No: %d\n", (int) ec_context->slavelist[i].eep_rev);
//            printf("Serial No: %d\n", (int) ec_context->slavelist[i].eep_ser);
//            printf("Topology: %d\n", (int) ec_context->slavelist[i].topology);
//            printf("Parent: %d\n", (int) ec_context->slavelist[i].parent);
//            printf("Configured Aderess still to be implemented\n");
//            }
            /* propagation delay is measured from first DC Slave*/
            fprintf(fp,"node_%d [label=\"%s\\nID: %d\\nSerialNr: %d\\nPropagation Delay: %d ns\"];\n",i,ec_context->slavelist[i].name,
                    ec_context->slavelist[i].eep_id, ec_context->slavelist[i].eep_ser, ec_context->slavelist[i].pdelay);
            fprintf(fp, "node_%d -> node_%d [label =\"%d ns\"];\n",(int)ec_context->slavelist[i].parent,i,ec_context->slavelist[i].pdelay - ec_context->slavelist[i -1].pdelay);

        }
        fprintf(fp, "}\n");

        fclose(fp);

        // execute the dot program to generate a graphViz image in a new process
//        printf("generate Topology image...\n");
	    int error = execv("/usr/bin/dot", dotArgv);
        if(error != 0){
            printf("Error in promodularSOEM.c cant create dotfile, make sure that dot executable is located in"
                   " /usr/bin/dot or change the path in execv function Error Code was: %d, that means %s\n", error, strerror(error));
            exit(-1);
        }
        exit(0);
    }


    return 0;
}

/**
 *
 * @param wkc The currently saved working counter
 * @param context The EtherCAT context
 * @return True if a change was detected, false if there was no change
 */
boolean detectTopologyChange(int wkc, ecx_contextt *context){
    /** Read Buffer 16 Bit*/
    uint16 r16;
    int wkcDetected;

    /* detect number of slaves */
    wkcDetected = ecx_BRD(context->port, 0x0000, ECT_REG_TYPE, sizeof(r16), &r16, EC_TIMEOUTSAFE);

 //   printf("Current WKC: %d\n", wkc);
 //   printf("Detected WKC: %d\n\n", wkcDetected);

//    printf("WKC current %d\n", wkc);
//    printf("WKC detected %d\n\n", wkcDetected);

    /* if the wkc changed, there was a topology change*/
    return wkc != wkcDetected;
}

char* getName(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].name;
}

int getVendorID(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_man;
}

int getProdCode(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_id;
}

int getRevNum(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_rev;
}

int getSerialNo(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_ser;
}

/**
 *
 * @param ec_context EtherCAT context
 * @param slave measured slave
 * @return delay from "first slave with dc" to "measured slave" in ns
 */
int getPropagationDelay(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].pdelay;
}

