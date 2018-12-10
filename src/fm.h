#ifndef __FM_H__
#define __FM_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
#include <ctime>
#include <algorithm>
#include <list>


#ifdef __cplusplus
extern "C"
#endif
{
  #include "standard.h"
  #include "parser.h"
  #include "util.h"
#ifdef __cplusplus
}
#endif

using namespace std;

class FM{

public:

    FM();
    FM(float e, float r);

private:
    struct group{
        int total_cell_size;
        //CELL* group_cellArray;
        int cellNum;
        list<CELL>* group_cellArray;
    } GROUP;

    // Temporary Datatype for sorting the CellArray
    struct celltype_temp{
        int id;
        int width, height;

        bool operator<(const celltype_temp& cell)
        {
            int area1, area2;
            area1 = this->width * this->height;
            area2 = cell.width * cell.height;
            return area1 < area2;
        };

    };


    group group1;
    group group2;
    int pmax;
    int total_cell_size;
    void Initialize();
    void Sort();
    int nc(NET net, int gid);

    void select_cell(int &candidate1, int &candidate2);


    
};

#endif
