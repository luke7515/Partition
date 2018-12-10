/*****************************************************************************
*   Title : 
*   Desc  : 
*   Author: 
*   Date  : 1995.2.14
****************************************************************************/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "benchtypes.h"

typedef struct nettable {
    SignalList *signal;
    int count;
    int i;
} NETTABLE;

void inputParse(const char *name);
void printCircuit();

#endif				// __PARSER_H__
