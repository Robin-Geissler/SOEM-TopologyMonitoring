//
// Created by robin on 23.11.20.
//

#include <stdio.h>

#include "promodularSOEM.h"

/** Arguments for dot program*/
char* dotArgv[] = {"dot", "-Tpng", "../../vizFiles/graphViz.gv", "-o", "../../vizFiles/graph.png"};


ecx_contextt *busMemberScan(char ioMap[]){
    printf("Scannig bus topology...\n");

    /* configure all slaves */
    if(ec_config(FALSE,ioMap) < 1){
        printf("No slaves found\n");
        /*exit here*/
    }
    printf("All slaves configured\n");

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
    printf("Finished reading out network configuration\n");
    return &ecx_context;
}

/* to get a png image do:
 * dot -Tpng graphViz.gv -o graph.png
 * */
int visualizeTopology(ecx_contextt *ec_context){

    FILE * fp;

    fp = fopen("../../vizFiles/graphViz.gv","w");

    fprintf(fp, "digraph G {\n\n");

    fprintf(fp, "node_0 [label=\"Master\"]\n");

    for(int i = 1; i <= *(ec_context->slavecount); i++){
        printf("Slave %d\n",i);
        printf( "Vendor ID: %x\n",(int)ec_context->slavelist[i].eep_man);
        printf("Product Code: %d\n",(int)ec_context->slavelist[i].eep_id);
        printf("Revision No: %d\n", (int)ec_context->slavelist[i].eep_rev);
        printf("Serial No: %d\n", (int)ec_context->slavelist[i].eep_ser);
        printf("Topology: %d\n",(int)ec_context->slavelist[i].topology);
        printf("Parent: %d\n", (int)ec_context->slavelist[i].parent);
        printf("Configured Aderess still to be implemented\n");

        fprintf(fp,"node_%d [label=\"%s\\nID: %d\\nSerialNr: %d\"];\n",i,ec_context->slavelist[i].name,
                ec_context->slavelist[i].eep_id, ec_context->slavelist[i].eep_ser);
        fprintf(fp, "node_%d -> node_%d;\n",(int)ec_context->slavelist[i].parent,i);

    }
    fprintf(fp, "}\n");

    fclose(fp);

    // execute the dot program to generate a graphViz image in a new process
    printf("generate Topology image...\n");
    if(fork() == 0){
        if(execv("/usr/bin/dot", dotArgv) != 0){
            printf("Error in promodularSOEM.c cant create dotfile\n");
            return -1;
        }
    }


    return 0;
}

