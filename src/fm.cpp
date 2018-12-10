#include "fm.h"

using namespace std;

FM::FM(){
    FM(0.1,0.5);
}

FM::FM(float e, float r){
    epsilon = e;
    ratio = r;
    Initialize();
    Compute();
}

void FM::Initialize(){
    Sort();
    group1.group_cellArray = new list<CELL>[pmax * 2 + 1]; // gid = 0
    group2.group_cellArray = new list<CELL>[pmax * 2 + 1]; // gid = 1

    // set te, fs

    update_gain();
    //CELL* cellptr;
    //for(int i = 0; i < netNum; i++){
    //    NET net = netArray[i];        
    //    if(nc(net, 0) == 0 || nc(net, 1) == 0){
    //        for(int j = 0; j < net.cellcount2; j++){
    //            cellptr = cellArray + net.cell2[j];
    //            cellptr->te++;
    //        }
    //    }
    //    if(nc(net, 0) == 1 || nc(net, 1) == 1){
    //        for(int j = 0; j < net.cellcount2; j++){
    //            cellptr = cellArray + net.cell2[j];
    //            cellptr->fs++;
    //        }
    //    }
    //}

    ////group1.maxgain = -pmax;
    ////group2.maxgain = -pmax;
    //for(int i = 0; i < cellNum; i++){
    //    cellArray[i].gain = cellArray[i].fs - cellArray[i].te;
    //    if(cellArray[i].gid == 0)
    //        group1.group_cellArray[cellArray[i].gain + pmax].push_back(cellArray[i]);
    //    else
    //        group2.group_cellArray[cellArray[i].gain + pmax].push_back(cellArray[i]);
    //}


}


void FM::Sort(){
    // Initialize array for sorting the CellArray
    celltype_temp* cellArrayTemp = new celltype_temp[cellNum];
    CELL* cellArraySorted = new CELL[cellNum];

    // get wmax and dmax while copying
    int wmax = 0;
    int dmax = 0;
    // Copy necessary values for sorting
    for(int i = 0; i < cellNum; i++){
        CELL* cell = cellArray +  i;

        // Copy data to Temp array
        cellArrayTemp[i].id = cell->id;
        cellArrayTemp[i].width = cell->width;
        cellArrayTemp[i].height = cell->height;
        
        int w = cell->width * cell->height;
        int d = cell->netcount2;
        if(wmax <= w)
            wmax = w;
        if(dmax <= d)
            dmax = d;
    }

    // sort the array with std lib
    sort(cellArrayTemp, cellArrayTemp + cellNum);

    // Map cellArray to Converted Array
    for(int i = 0; i < cellNum; i++){
        int cell_id = cellArrayTemp[i].id;
        cellArraySorted[i] = cellArray[cell_id];
    }    
    // set gid for each index
    group1.total_cell_size = 0;
    group2.total_cell_size = 0;
    total_cell_size = 0;

    for(int i = 0; i < cellNum; i++){
        int id = cellArraySorted[i].id;
        int area = cellArray[id].width * cellArray[id].height;
        if((id % 2) == 0){
            cellArray[id].gid = 0;
            group1.total_cell_size += area;
            
        }
        else{
            cellArray[id].gid = 1;
            group2.total_cell_size += area;
        }
        cellArray[id].locked = 0;
        total_cell_size += area;
    }

    delete [] cellArraySorted;
    delete [] cellArrayTemp;

    pmax = wmax * dmax;
}

int FM::nc(NET net, int gid){
    int count = 0;
    CELL *cellptr;
    for(int i = 0; i < net.cellcount2; i++){        
        cellptr = cellArray + net.cell2[i];
        if(cellptr->gid == gid)
            count++;
    }
    return count;
}

int FM::select_cell(){
    
    int g1 = -1;
    int g2 = -1;
    int gain;

    gain = -pmax;

    for(int i = 2*pmax; i >= 0; i--){
        list<CELL> array_temp = group1.group_cellArray[i];
        if(array_temp.size() > 0){
            std::list<CELL>::iterator it;
            for (it = array_temp.begin(); it != array_temp.end(); ++it){
                if(!it->locked){
                    int area = it->width * it->height;
                    if(group1.total_cell_size - area 
                        >= total_cell_size * (ratio - epsilon/2)){
                        g1 = it->id;
                        gain = it->gain;
                        break;
                    }
                }
            }            
        }
    }


    for(int i = 2*pmax; i >= 0; i--){
        list<CELL> array_temp = group2.group_cellArray[i];
        if(group2.group_cellArray[i].size() > 0){
            std::list<CELL>::iterator it;
            for (it = array_temp.begin(); it != array_temp.end(); ++it){
                if(!it->locked){
                    int area = it->width * it->height;
                    if(group2.total_cell_size - area 
                        >= total_cell_size * (ratio - epsilon/2)){
                        g2 = it->id;
                        if(it->gain < gain)
                            return g1;
                        else
                            return g2;                        
                    }
                }
            }        
        }
    }

    return g1;
    
}

void FM::update_gain()
{
    CELL* cellptr;
    for(int i = 0; i < cellNum; i++){
        cellptr = cellArray + i;
        cellptr->te = 0;
        cellptr->fs = 0;
    }
    for(int i = 0; i < netNum; i++){
        NET net = netArray[i];        
        if(nc(net, 0) == 0 || nc(net, 1) == 0){
            for(int j = 0; j < net.cellcount2; j++){
                cellptr = cellArray + net.cell2[j];
                cellptr->te++;
            }
        }
        if(nc(net, 0) == 1 || nc(net, 1) == 1){
            for(int j = 0; j < net.cellcount2; j++){
                cellptr = cellArray + net.cell2[j];
                cellptr->fs++;
            }
        }
    }

    //group1.maxgain = -pmax;
    //group2.maxgain = -pmax;
    for(int i = 0; i < cellNum; i++){
        cellArray[i].gain = cellArray[i].fs - cellArray[i].te;
        if(cellArray[i].gid == 0)
            group1.group_cellArray[cellArray[i].gain + pmax].push_back(cellArray[i]);
        else
            group2.group_cellArray[cellArray[i].gain + pmax].push_back(cellArray[i]);
    }
}

void FM::update_cell(int id)
{
    CELL* cell = cellArray + id;
    cell->locked = 1;

    


}

void FM::Compute(){




}

