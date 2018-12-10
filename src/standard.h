/****************************************************************************
*	Title : standard.h
*	Desc  : 
*	Author: Jeon Jinhwan
*	Date  : 1995.2.13
*****************************************************************************/

#ifndef __STANDARD_H__
#define __STANDARD_H__

#include <stdio.h>
#include <stdbool.h>

#define		MAXNAMELEN		30

/* module type */
#define		STANDARD		1
#define		PAD				2
#define		GENERAL			3
#define		FEEDTHROUGH		5

/* cell orientation */
#define		ROT0			1
#define		ROT90			2
#define		ROT180			3
#define		ROT270			4
#define		YREFLECTION		0x100
#define		XREFLECTION 	0x200

/* padside */
#define 	BOTTOM 			1
#define 	RIGHT  			2
#define 	TOP    			3
#define 	LEFT   			4
#define 	NOSIDE 			5

/* modlue structure */
typedef struct moduletype {
    char name[MAXNAMELEN];
    int type;
    int left;
    int right;
    int top;
    int bottom;
    short pincount;
    struct pintype *pin;
} MODULE;

typedef struct celltype {
    int id;
    char name[MAXNAMELEN];
    int width, height;
    short netcount2;
    short *net2;

    short module;
    short netcount;
    short *net;

    int gid;
    int gain;
    int fs;
    int te;
    bool locked;
    /* pad information */
    short padside;
    double padpos;
} CELL;


typedef struct pintype {
    char name[MAXNAMELEN];
    int x, y;
    int layer;
    struct pintype *equiv;
} PIN;

typedef struct nettype {
    char name[MAXNAMELEN];
    short cellcount2;
    short *cell2;

    short cellcount;
    short *cell;

    short *pin;
} NET;

/* statistical informations */
typedef struct stattype {
    int max_net_per_cell;
    int max_cell_per_net;
    double avg_net_per_cell;
    double avg_cell_per_net;
    int max_cell_width;
    int min_cell_width;
    int max_cell_height;
    int min_cell_height;
    double avg_cell_width;
    double avg_cell_height;
    int feed_width;
    int max_cell_size;
    int min_cell_size;
    double avg_cell_size;

    /* standard deviation */
    double sig_net_per_cell;
    double sig_cell_per_net;
    double sig_cell_width;
    double sig_cell_height;
    double sig_cell_size;
} STAT;

extern CELL *cellArray;
extern NET *netArray;
extern MODULE *moduleArray;
extern STAT stat;

extern int cellNum;
extern int padNum;
extern int netNum;
extern int moduleNum;

extern short *tempnets;

/* utility functions */

/*
void  errorMsg();
void* mem_alloc();
FILE* file_open();
int   randomize();
double get_utime();
double get_stime();
*/
#define		RAND()			rand()
#define		SRAND(seed)		srand(seed)

#endif
