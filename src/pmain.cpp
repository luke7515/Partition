/****************************************************************************
*   Title : 
*   Desc  : 
*   Author: 
*   Date  : 
*****************************************************************************/

#include <stdio.h>
#include "fm.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "standard.h"
#include "parser.h"
#include "util.h"

#ifdef __cplusplus
}
#endif


static void part(char *file)
{
    inputParse(file);

    /* test print, you may delete this line */
    //printCircuit();

	/*********************************
	* insert your partition code here 
	**********************************/
    FM(0.1, 0.5);
}

int main(int argc, char **argv)
{
    //if (argc < 2)
	//errorMsg("usage: part input");

    char filename[20] = "../test/primary2";

    part(filename);
    return 0;
}
