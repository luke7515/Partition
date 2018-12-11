#include "fm.h"

using namespace std;

FM::FM(){
    FM(0.1,0.5);
}

FM::FM(float e, float r){
    epsilon = e;
    ratio = r;
    total_gain = 0;
    Initialize();
    Compute();
}

void FM::Initialize(){
    Sort();
    group1.group_cellArray = new list<CELL>[pmax * 2 + 1]; // gid = 0
    group2.group_cellArray = new list<CELL>[pmax * 2 + 1]; // gid = 1

    // set te, fs
    update_gain();

}


void FM::Sort(){
    // Initialize array for sorting the CellArray
    celltype_temp* cellArrayTemp = new celltype_temp[cellNum];
    CELL* cellArraySorted = new CELL[cellNum];

    // get wmax and dmax while copying
    int dmax = 0;
    // Copy necessary values for sorting
    for(int i = 0; i < cellNum; i++){
        CELL* cell = cellArray +  i;

        // Copy data to Temp array
        cellArrayTemp[i].id = cell->id;
        cellArrayTemp[i].width = cell->width;
        cellArrayTemp[i].height = cell->height;
        
        int d = cell->netcount2;
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

    pmax = dmax;
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
    int gain1;
    int gain2;

    gain1 = -pmax;
    gain2 = -pmax;

    for(int i = 2*pmax; i >= 0; i--){
        list<CELL> array_temp = group1.group_cellArray[i];
        if(array_temp.size() > 0){
            std::list<CELL>::iterator it;
            for (it = array_temp.begin(); it != array_temp.end(); ++it){
                if(!it->locked){
                    int area = it->width * it->height;
                    if(group1.total_cell_size - area 
                        >= total_cell_size * (ratio - epsilon)){
                        if(gain1 < it->gain){
                            g1 = it->id;
                            gain1 = it->gain;
                            it = array_temp.end();
                            i = -1;
                        }
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
                        >= total_cell_size * (ratio - epsilon)){
                        if(gain2 < it->gain){
                            g2 = it->id;
                            gain2 = it->gain;
                            it = array_temp.end();
                            i = -1;
                        }
                    }
                }
            }        
        }
    }

    
    if(gain1 < gain2)
        return g2;

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
        for(int j = 0; j < net.cellcount2; j++){
            cellptr = cellArray + net.cell2[j];
            if(nc(net, 1-cellptr->gid) == 0)
                cellptr->te++;
        }
        for(int j = 0; j < net.cellcount2; j++){
            cellptr = cellArray + net.cell2[j];
            if(nc(net, cellptr->gid) == 1)
                cellptr->fs++;
        }
    }

    int maxgain = -pmax;
    for(int i = 0; i < cellNum; i++){
        if(!cellArray[i].locked){
            cellArray[i].gain = cellArray[i].fs - cellArray[i].te;
            if(cellArray[i].gain > maxgain)
                maxgain = cellArray[i].gain;
        }
    }
    if(maxgain > pmax){
        delete [] group1.group_cellArray;
        delete [] group2.group_cellArray;
        group1.group_cellArray = new list<CELL>[maxgain * 2 + 1]; // gid = 0
        group2.group_cellArray = new list<CELL>[maxgain * 2 + 1]; // gid = 1
        pmax = maxgain;
    }

    for(int i = 0; i < cellNum; i++){
        if(!cellArray[i].locked){

            if(cellArray[i].gid == 0)
                group1.group_cellArray[cellArray[i].gain + pmax].push_back(cellArray[i]);
            else
                group2.group_cellArray[cellArray[i].gain + pmax].push_back(cellArray[i]);
        }
    }

    
}

void FM::update_cell(int id)
{
    CELL* cell = cellArray + id;
    cell->locked = 1;
    int area = cell->width * cell->height;
    if(cell->gid == 0){
        cell->gid = 1;
        group1.total_cell_size -= area;
        group2.total_cell_size += area;
    }
    else{
        cell->gid = 0;
        group1.total_cell_size += area;
        group2.total_cell_size -= area;
    }   
    update_gain();
}

void FM::Compute(){

    CELL* cellptr;

    //for(int i = 0; i <  cellNum; i++){
    //    cellptr = cellArray + i;
    //    cout << cellptr->name << " " << cellptr->gain << endl;
    //}

    int iter = 0;
    Calculate_Cost();

    for(int i = 0; i < cellNum; i++){
        cout << "iterate" << iter  << " = " << total_gain << endl;
        iter++;
        int id = select_cell();
        cellptr = cellArray + id;
        cellptr = cellArray + id;
        if(id == -1)
            break;
        total_gain += cellptr->gain;
        
        for(int i = 0; i < cellNum; i++){
            cellptr = cellArray + i;
            if(!group1.group_cellArray[cellptr->gain + pmax].empty()){
                group1.group_cellArray[cellptr->gain + pmax].pop_back();
            }
            if(!group2.group_cellArray[cellptr->gain + pmax].empty()){
                group2.group_cellArray[cellptr->gain + pmax].pop_back();
            
            }
        }

        update_cell(id);
    }

    //Calculate_Cost();
    std::cout << "iterate" << iter << " = " << total_gain << endl;



}

void FM::Calculate_Cost(){
    CELL* cellptr;
    NET* netptr;
    int cost = 0;
    for(int i = 0; i < netNum; i++){
        int temp = -1;
        netptr = netArray + i;
        for(int j = 0; j < netptr->cellcount2; j++){
            cellptr = cellArray + (netptr->cell2)[j];
            if(temp == -1){
                temp = cellptr->gid;
            }
            else if(temp != cellptr->gid){
                cost++;
                j = netptr->cellcount2;
            }        
        }    
    
    }
    cout << "value = " << cost << endl;


}
