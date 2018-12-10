/****************************************************************************
*	Title : parser.c
*	Desc  : Input Parser
*	Author: Jinhwan Jeon
*	Date  : 1995.2.15
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include "benchtypes.h"
#include "parser.h"
#include "hash.h"
#include "standard.h"
#include "util.h"

/*
#define 	COUNT2
*/

static ModuleList *parModule = 0;
static ModuleList *subModule = 0;
static HASH nethash;

static void freeIOList(IOList * head)
{
    IOList *iop, *iopl;
    iop = head;
    while (iop) {
	iopl = iop->Link;
	free(iop);
	iop = iopl;
    }
}

static void freeSignalList(SignalList * head)
{
    SignalList *sp, *spl;
    sp = head;
    while (sp) {
	spl = sp->Link;
	free(sp);
	sp = spl;
    }
}

static void freeNetworkList(NetworkList * head)
{
    NetworkList *nwp, *nwpl;
    nwp = head;
    while (nwp) {
	nwpl = nwp->Link;
	freeSignalList(nwp->SignalListHead);
	free(nwp);
	nwp = nwpl;
    }
}

static void freePlacementList(PlacementList * head)
{
    PlacementList *pp, *ppl;
    pp = head;
    while (pp) {
	ppl = pp->Link;
	free(pp);
	pp = ppl;
    }
}

static void freeModuleList(ModuleList * head)
{
    ModuleList *modpl, *modp = head;

    while (modp) {
	modpl = modp->Link;
	freeIOList(modp->IOListHead);
	freeNetworkList(modp->NetworkListHead);
	freePlacementList(modp->PlacementListHead);
	free(modp);
	modp = modpl;
    }
}

static void readModuleList(FILE * input)
{
    ModuleList module;
    ModuleList *modp, *mod;
    NetworkList *nwp;

    module.Link = 0;
    modp = 0;
    moduleNum = 0;
    cellNum = 0;
    while (1) {
	GetModule(input, module.Name, &module.ModType,
		  &module.Width, &module.Height,
		  &module.IOListHead,
		  &module.NetworkListHead, &module.PlacementListHead);
	if (module.ModType == ENDFILE)
	    break;
	mod = (ModuleList *) mem_calloc(sizeof(ModuleList), 1);
	*mod = module;

	if (module.ModType == PARENT) {
	    if (parModule != 0)
		errorMsg("Error: more than one parent module");
	    parModule = mod;
	    nwp = mod->NetworkListHead;
	    while (nwp) {
		cellNum++;
		nwp = nwp->Link;
	    }
	} else {
	    moduleNum++;
	    if (subModule == 0)
		modp = subModule = mod;
	    else {
		modp->Link = mod;
		modp = mod;
	    }
	}
    }
}

ModuleList *findModule(NameType name)
{
    ModuleList *modp;
    modp = subModule;

    while (modp) {
	if (!strcmp(modp->Name, name))
	    return modp;
	modp = modp->Link;
    }
    return modp;
}

IOList *findIOList(IOList * head, NameType name)
{
    IOList *iop;
    iop = head;
    while (iop) {
	if (!strcmp(iop->SignalName, name))
	    break;
	iop = iop->Link;
    }
    return iop;
}

int net_hashfunc(NETTABLE * net)
{
    return default_hashfunc(net->signal->SignalName);
}

int net_hashcmp(NETTABLE * net1, NETTABLE * net2)
{
    return default_hashcmp(net1->signal->SignalName,
			   net2->signal->SignalName);
}

void checkSemantics()
{
    ModuleList *modp;
    NetworkList *nwp;
    SignalList *sigp;
    NETTABLE nett, *nettp;

    nwp = parModule->NetworkListHead;
    padNum = 0;
    netNum = 0;

    init_hash(&nethash, 0, 0, (cellNum / 10 > 2) ? cellNum / 10 : 2);
    nethash.func = net_hashfunc;
    nethash.cmp = net_hashcmp;

    /* check semantics for modules and signals */
    while (nwp) {
	if ((modp = findModule(nwp->ModuleName)) == 0) {
	    errorMsg("Error: undefined module %s", nwp->ModuleName);
	}
	if (modp->ModType == PAD) {
	    padNum++;
	}

	sigp = nwp->SignalListHead;
	while (sigp) {
	    nett.signal = sigp;
	    if ((nettp = hash_find(&nethash, &nett)) == 0) {
		netNum++;
		nett.count = 1;
		nettp = mem_alloc(sizeof(NETTABLE));
		*nettp = nett;
		hash_insert(&nethash, nettp);
	    } else {
		nettp->count++;
	    }
	    sigp = sigp->Link;
	}
	nwp = nwp->Link;
    }

    /* check single ended net */
    /*
       for( i=0; i<nethash.len; i++ ){
       hashp = nethash.hdr+i;
       while(hashp && hashp->data){
       nettp = hashp->data;
       if( nettp->count == 1 ){
       printf( "Warning: single ended signal %s\n",
       nettp->signal->SignalName );
       }
       hashp = hashp->next;
       }
       }
     */
}

void initModule()
{
    int i;
    int pinCount;
    char *lastName;
    ModuleList *modp;
    IOList *iop;
    PIN *pinptr;
    int x, y;
    int left, top;
    int pin;

    modp = subModule;
    i = 0;

    while (modp) {
	strcpy(moduleArray[i].name, modp->Name);
	left = -modp->Width / 2;
	top = -modp->Height / 2;
	moduleArray[i].left = left;
	moduleArray[i].right = -left;
	moduleArray[i].top = top;
	moduleArray[i].bottom = -top;
	moduleArray[i].type = modp->ModType;

	/* count pin number */
	iop = modp->IOListHead;
	lastName = 0;
	pinCount = 0;
	while (iop) {
	    if (!lastName || strcmp(iop->SignalName, lastName))
		pinCount++;
	    lastName = iop->SignalName;
	    iop = iop->Link;
	}
	moduleArray[i].pincount = pinCount;
	moduleArray[i].pin = (PIN *) mem_calloc(sizeof(PIN), pinCount);

	/* copy pin informations */
	iop = modp->IOListHead;
	lastName = 0;
	pin = 0;
	while (iop) {
	    switch (iop->Side) {
	    case BOTTOM:
		x = left + iop->Position;
		y = -top;
		break;
	    case RIGHT:
		x = -left;
		y = top + iop->Position;
		break;
	    case TOP:
		x = left + iop->Position;
		y = top;
		break;
	    case LEFT:
		x = left;
		y = top + iop->Position;
		break;
	    }
	    if (!lastName || strcmp(iop->SignalName, lastName)) {
		pinptr = &moduleArray[i].pin[pin];
		strcpy(pinptr->name, iop->SignalName);
		pinptr->x = x;
		pinptr->y = y;
		pin++;
	    } else {
		pinptr = pinptr->equiv =
		    (PIN *) mem_calloc(sizeof(PIN), 1);
		strcpy(pinptr->name, iop->SignalName);
		pinptr->x = x;
		pinptr->y = y;
	    }
	    lastName = iop->SignalName;
	    iop = iop->Link;
	}
	modp = modp->Link;
	i++;
    }
    if (i != moduleNum)
	errorMsg("Error: initModule() mismatch");
}

void initNet()
{
    HASHTABLE *h;
    NETTABLE *nett;
    int i;
    int net = 0;

    /* copy network information to netArray */
    for (i = 0; i < nethash.len; i++) {
	h = nethash.hdr + i;
	while (h && h->data) {
	    nett = h->data;
	    strcpy(netArray[net].name, nett->signal->SignalName);
	    netArray[net].cellcount = nett->count;
	    netArray[net].cell =
		(short *) mem_calloc(sizeof(short), nett->count);
	    nett->i = net++;
	    nett->count = 0;
	    h = h->next;
	}
    }
    if (net != netNum)
	errorMsg("Error: initNet() mismatch");
}

int findModuleIndex(char *name)
{
    int i;
    for (i = 0; i < moduleNum; i++) {
	if (!strcmp(name, moduleArray[i].name))
	    return i;
    }
    return -1;
}

void initCell()
{
    NetworkList *nwp;
    int i, modi;
    int cell, pad;
    IOList *iop;
    SignalList *sigp;
    int netCount;
    NETTABLE nett, *nettp;

    cell = 0;
    pad = cellNum;
    nwp = parModule->NetworkListHead;
    while (nwp) {
	/* find moudule index in moduleArray */
	modi = findModuleIndex(nwp->ModuleName);

	if (modi == -1)
	    errorMsg("Error: initCell()");

	/* if module is PAD, get pad position */
	if (moduleArray[modi].type == PAD) {
	    sigp = nwp->SignalListHead;
	    while (sigp) {
		iop = findIOList(parModule->IOListHead, sigp->SignalName);
		if (iop) {
		    cellArray[pad].padside = iop->Side;
		    cellArray[pad].padpos = iop->Position;
		    break;
		}
		sigp = sigp->Link;
	    }
	    i = pad++;
	} else
	    i = cell++;

	/* copy other informations */
	strcpy(cellArray[i].name, nwp->InstanceName);
	cellArray[i].id = i;
	cellArray[i].module = modi;
	cellArray[i].width = 2 * moduleArray[cellArray[i].module].right;
	cellArray[i].height = 2 * moduleArray[cellArray[i].module].bottom;

	/* get connected net number */
	sigp = nwp->SignalListHead;
	netCount = 0;

	while (sigp) {
	    netCount++;
	    sigp = sigp->Link;
	}
	cellArray[i].netcount = netCount;
	cellArray[i].net = (short *) mem_calloc(sizeof(short), netCount);

	/* set connected net index */
	sigp = nwp->SignalListHead;
	netCount = 0;

	while (sigp) {
	    nett.signal = sigp;
	    nettp = hash_find(&nethash, &nett);

	    /* set connected cell index for the net */
	    netArray[nettp->i].cell[nettp->count++] = i;
	    /* set connected pin index for the cell */
	    /*
	       netArray[nettp->i].pin[nettp->count++] = netCount;
	     */

	    /* set connected net index for the cell */
	    cellArray[i].net[netCount++] = nettp->i;

	    sigp = sigp->Link;
	}
	nwp = nwp->Link;
    }
    if (cell != cellNum || pad != cellNum + padNum)
	errorMsg("Error: initCell() mismatch");
}

void initNetCell2()
{
    int i, j;
    int count;
    short *temp, *net, *cell;

    temp = mem_calloc(sizeof(short), cellNum + padNum > netNum ?
		      cellNum + padNum : netNum);
    net = mem_calloc(sizeof(short), netNum);
    cell = mem_calloc(sizeof(short), cellNum);

    for (i = 0; i < cellNum + padNum; i++) {
	count = 0;
	for (j = 0; j < cellArray[i].netcount; j++) {
	    if (temp[cellArray[i].net[j]] == 0) {
		net[count++] = cellArray[i].net[j];
		temp[cellArray[i].net[j]] = 1;
	    }
	}
	cellArray[i].netcount2 = count;
	if (count) {
	    cellArray[i].net2 = mem_calloc(sizeof(short), count);
	    for (j = 0; j < count; j++) {
		temp[net[j]] = 0;
		cellArray[i].net2[j] = net[j];
	    }
	}
    }

    for (i = 0; i < netNum; i++) {
	count = 0;
	for (j = 0; j < netArray[i].cellcount; j++) {
	    if (temp[netArray[i].cell[j]] == 0) {
		if (netArray[i].cell[j] >= cellNum)
		    continue;
		cell[count++] = netArray[i].cell[j];
		temp[netArray[i].cell[j]] = 1;
	    }
	}
	netArray[i].cellcount2 = count;
	if (count) {
	    netArray[i].cell2 = mem_calloc(sizeof(short), count);
	    for (j = 0; j < count; j++) {
		temp[cell[j]] = 0;
		netArray[i].cell2[j] = cell[j];
	    }
	}
    }

    free(cell);
    free(net);
    free(temp);
}

void getStat(int num)
{
    int i;
    int size;
    double net_num, cell_num;
    double tot_width, tot_height;
    double tot_size;
    double sq_net_num, sq_cell_num;
    double sq_tot_width, sq_tot_height;
    double sq_tot_size;
    CELL *cellptr;
    NET *netptr;

    stat.max_cell_per_net = 0;
    stat.max_net_per_cell = 0;
    stat.max_cell_size = 0;

    stat.min_cell_width = cellArray[0].width;
    stat.min_cell_height = cellArray[0].height;

    stat.min_cell_size = cellArray[0].height * cellArray[0].width;

    net_num = 0.0;
    cell_num = 0.0;
    tot_width = 0.0;
    tot_height = 0.0;
    tot_size = 0.0;

    sq_net_num = 0.0;
    sq_cell_num = 0.0;
    sq_tot_width = 0.0;
    sq_tot_height = 0.0;
    sq_tot_size = 0.0;

    for (i = 0; i < num; i++) {
	cellptr = cellArray + i;
	size = cellptr->width * cellptr->height;

#ifndef COUNT2
	net_num += cellptr->netcount;
#else
	net_num += cellptr->netcount2;
#endif

	tot_width += cellptr->width;
	tot_height += cellptr->height;
	tot_size += size;

#ifndef COUNT2
	sq_net_num += cellptr->netcount * cellptr->netcount;
#else
	sq_net_num += cellptr->netcount2 * cellptr->netcount2;
#endif

	sq_tot_width += cellptr->width * cellptr->width;
	sq_tot_height += cellptr->height * cellptr->height;
	sq_tot_size += (double) size *(double) size;

#ifndef COUNT2
	if (stat.max_net_per_cell < cellptr->netcount)
	    stat.max_net_per_cell = cellptr->netcount;
#else
	if (stat.max_net_per_cell < cellptr->netcount2)
	    stat.max_net_per_cell = cellptr->netcount2;
#endif
	if (stat.max_cell_height < cellptr->height)
	    stat.max_cell_height = cellptr->height;
	if (stat.max_cell_width < cellptr->width)
	    stat.max_cell_width = cellptr->width;
	if (stat.min_cell_height > cellptr->height)
	    stat.min_cell_height = cellptr->height;
	if (stat.min_cell_width > cellptr->width)
	    stat.min_cell_width = cellptr->width;
	if (stat.min_cell_size > size)
	    stat.min_cell_size = size;
	if (stat.max_cell_size < size)
	    stat.max_cell_size = size;
    }
    stat.avg_net_per_cell = net_num / (double) (num);
    stat.avg_cell_height = tot_height / (double) (num);
    stat.avg_cell_width = tot_width / (double) (num);
    stat.avg_cell_size = tot_size / (double) (num);

    stat.sig_net_per_cell = sqrt(sq_net_num / (double) num -
				 stat.avg_net_per_cell *
				 stat.avg_net_per_cell);
    stat.sig_cell_height =
	sqrt(sq_tot_height / (double) num -
	     stat.avg_cell_height * stat.avg_cell_height);
    stat.sig_cell_width =
	sqrt(sq_tot_width / (double) num -
	     stat.avg_cell_width * stat.avg_cell_width);
    /*
       stat.sig_cell_size = sqrt(sq_tot_size / (double)num - 
       stat.avg_cell_size * stat.avg_cell_size);
     */

    sq_tot_size = 0.0;
    for (i = 0; i < num; i++) {
	cellptr = cellArray + i;
	size = cellptr->width * cellptr->height;
	sq_tot_size += (size - stat.avg_cell_size) *
	    (size - stat.avg_cell_size);
    }
    stat.sig_cell_size = sqrt(sq_tot_size / (double) num);

    for (i = 0; i < netNum; i++) {
	netptr = netArray + i;

#ifndef COUNT2
	cell_num += netptr->cellcount;
	sq_cell_num += netptr->cellcount * netptr->cellcount;
	if (stat.max_cell_per_net < netptr->cellcount)
	    stat.max_cell_per_net = netptr->cellcount;
#else
	cell_num += netptr->cellcount2;
	sq_cell_num += netptr->cellcount2 * netptr->cellcount2;
	if (stat.max_cell_per_net < netptr->cellcount2)
	    stat.max_cell_per_net = netptr->cellcount2;
#endif
    }
    stat.avg_cell_per_net = cell_num / (double) netNum;
    stat.sig_cell_per_net = sq_cell_num / (double) netNum -
	stat.avg_cell_per_net * stat.avg_cell_per_net;

    for (i = 0; i < moduleNum; i++) {
	if (moduleArray[i].type == FEEDTHROUGH) {
	    stat.feed_width = 2 * moduleArray[i].right;
	}
    }
}

void inputParse(const char *fileName)
{
    FILE *ifp;
    char namebuf[80];

    if (!fileName)
	ifp = stdin;
    else {
	sprintf(namebuf, "%s.yal", fileName);
	ifp = file_open(namebuf, "rt");
    }

    readModuleList(ifp);

    if (!parModule)
	errorMsg("Error: no parent module");
    if (!parModule->NetworkListHead)
	errorMsg("Error: no network list");

    /* get statistics and check semantics */
    checkSemantics();
    cellNum -= padNum;

    cellArray = mem_calloc(sizeof(CELL), (cellNum + padNum));
    netArray = mem_calloc(sizeof(NET), netNum);
    moduleArray = mem_calloc(sizeof(MODULE), moduleNum);

    /* Build data structure */

    /* copy module infomation to moduleArray */
    initModule();

    /* copy signal information to netArray */
    initNet();

    /* copy cell information to cellArray */
    initCell();

    initNetCell2();
    /* get statistics */
    getStat(cellNum);

    /* free allocated memory */
    freeModuleList(subModule);
    freeModuleList(parModule);
    close_hash(&nethash);
}

void printCell(CELL * cellptr)
{
    int i;
    printf("%s%s : ", cellptr->name,
	   moduleArray[cellptr->module].type == PAD ? "(pad)" : "");

    for (i = 0; i < cellptr->netcount2; i++)
	printf("%s ", netArray[cellptr->net2[i]].name);
    printf("\n");
}

void printNet(NET * netptr)
{
    int i;
    printf("%s : ", netptr->name);

    for (i = 0; i < netptr->cellcount2; i++)
	printf("%s%s ", cellArray[netptr->cell2[i]].name,
	       netptr->cell2[i] >= cellNum ? "(pad)" : "");
    printf("\n");
}

void printCircuit()
{
    int i;

    printf("*** cell list ***\n");
    for (i = 0; i < cellNum; i++)
	printCell(cellArray + i);

    printf("*** net list ***\n");
    for (i = 0; i < netNum; i++)
	printNet(netArray + i);
}
