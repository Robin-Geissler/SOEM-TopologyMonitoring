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




#endif //SOEM_PROMODULARSOEM_H
