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

/**
 * @brief updates the gv file where the topology is saved
 * @param ec_context context where the detected topology is saved
 * @param idMin all Slaves with IDs < idMin will not be considered
 * @param idMax all Slaves with IDs > idMax will not be considered
 * @return 0 if dot file was created sucessfully
 */
int updateTopology(ecx_contextt *ec_context, int idMin, int idMax) {
    // Generation of dot file happens in new process
    if (fork() == 0) {

        FILE *fp;

        fp = fopen("../../vizFiles/graphViz.gv", "w");

        fprintf(fp, "digraph G {\n\n");

        fprintf(fp, "node_0 [label=\"Master\"]\n");

        for (int i = 1; i <= *(ec_context->slavecount); i++) {
            if (i >= idMin && i <= idMax) {
                /* propagation delay is measured from first DC Slave*/
                fprintf(fp, "node_%d [label=\"%s\\nID: %d\\nSerialNr: %d\\nPropagation Delay: %d ns\"];\n", i,
                        ec_context->slavelist[i].name,
                        ec_context->slavelist[i].eep_id, ec_context->slavelist[i].eep_ser,
                        ec_context->slavelist[i].pdelay);
                fprintf(fp, "node_%d -> node_%d [label =\"%d ns\"];\n", (int) ec_context->slavelist[i].parent, i,
                        ec_context->slavelist[i].pdelay - ec_context->slavelist[i - 1].pdelay);
            }
        }
        fprintf(fp, "}\n");

        fclose(fp);

        exit(0);
    }
    return 0;
}

/* to get a png image do:
 * dot -Tpng graphViz.gv -o graph.png
 * */

/**
 * @brief updates the gv file where the topology is saved and updates the png file where the topology is visualized
 * @param ec_context context where the detected topology is saved
 * @param idMin all Slaves with IDs < idMin will not be considered
 * @param idMax all Slaves with IDs > idMax will not be considered
 * @return 0 if dot file was created sucessfully
 */
int visualizeTopology(ecx_contextt *ec_context, int idMin, int idMax){
    // Visualization happens in new Thread to not delay main process
    if(fork() == 0){

        FILE * fp;

        fp = fopen("../../vizFiles/graphViz.gv","w");

        fprintf(fp, "digraph G {\n\n");

        fprintf(fp, "node_0 [label=\"Master\"]\n");

        for(int i = 1; i <= *(ec_context->slavecount); i++){
            if(i >= idMin && i <= idMax) {
                /* propagation delay is measured from first DC Slave*/
                fprintf(fp, "node_%d [label=\"%s\\nID: %d\\nSerialNr: %d\\nPropagation Delay: %d ns\"];\n", i,
                        ec_context->slavelist[i].name,
                        ec_context->slavelist[i].eep_id, ec_context->slavelist[i].eep_ser,
                        ec_context->slavelist[i].pdelay);
                fprintf(fp, "node_%d -> node_%d [label =\"%d ns\"];\n", (int) ec_context->slavelist[i].parent, i,
                        ec_context->slavelist[i].pdelay - ec_context->slavelist[i - 1].pdelay);
            }
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
 * @param ec_context EtherCAT context
 * @param slave measured slave
 * @return name of the slave
 */
char* getName(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].name;
}

/**
 *
 * @param ec_context EtherCAT context
 * @param slave measured slave
 * @return vendor id  of the slave
 */
int getVendorID(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_man;
}

/**
 *
 * @param ec_context EtherCAT context
 * @param slave measured slave
 * @return product code  of the slave
 */
int getProdCode(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_id;
}

/**
 *
 * @param ec_context EtherCAT context
 * @param slave measured slave
 * @return revision number of the slave
 */
int getRevNum(ecx_contextt *ec_context, int slave){
    if(*(ec_context->slavecount) < slave - 1) printf("slave dose not exist");
    return ec_context->slavelist[slave].eep_rev;
}

/**
 *
 * @param ec_context EtherCAT context
 * @param slave measured slave
 * @return serial number of the slave
 */
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

/**
 *
 * @param ec_context EtherCAT context
 * @return number of slaves saved in ec_context in the network
 */
int getSlaveNumber(ecx_contextt *ec_context){
    return *(ec_context->slavecount);
}

