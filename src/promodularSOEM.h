//
// Created by robin on 23.11.20.
//

#ifndef SOEM_PROMODULARSOEM_H
#define SOEM_PROMODULARSOEM_H

#include <stdio.h>

// TODO Linux Include markieren und Präprozesor befehle setzen
#include <unistd.h>


#include "ethercat.h"



ecx_contextt *busMemberScan(char ioMap[]);
int visualizeTopology(ecx_contextt *ec_context);

char *getName(ecx_contextt *ec_context, int slave);
int getVendorID(ecx_contextt *ec_context, int slave);
int getProdCode(ecx_contextt *ec_context, int slave);
int getRevNum(ecx_contextt *ec_context, int slave);
int getSerialNo(ecx_contextt *ec_context, int slave);


#endif //SOEM_PROMODULARSOEM_H
