//
// Created by robin on 23.11.20.
//

#ifndef SOEM_PROMODULARSOEM_H
#define SOEM_PROMODULARSOEM_H

#include <stdio.h>

// TODO Linux Include markieren und Präprozesor befehle setzen
#include <unistd.h>


#include "ethercat.h"

/** Time to wait before each busMemberScan in micro seconds
 * @value XMC_ESC = 0
 * @value EK1100 = 0
 * @value EK1100 + EL1808 = 250000
 * */
#define SLAVE_CONFIG_TIME_us 0

ecx_contextt *busMemberScan(char ioMap[], int *wkc);
int visualizeTopology(ecx_contextt *ec_context);

boolean detectTopologyChange(int wkc, ecx_contextt *context);

char *getName(ecx_contextt *ec_context, int slave);
int getVendorID(ecx_contextt *ec_context, int slave);
int getProdCode(ecx_contextt *ec_context, int slave);
int getRevNum(ecx_contextt *ec_context, int slave);
int getSerialNo(ecx_contextt *ec_context, int slave);


#endif //SOEM_PROMODULARSOEM_H
