//
// Created by athena on 8/12/19.
//
#include "UtilQE.h"
#include "QueryEditor.h"
//uint64_t getResults(RelationMD *CorrespondingBinding, int PredicateParts[4]);

uint64_t * HandleSameColumnException(int *PParts, RelationMD *Binding, IMData *imData);

using namespace std;
void initializeIMData(IMData * imData, int numOfBindings){
    imData->numOfBindings = numOfBindings;
    imData->visited = new bool[numOfBindings];
    for (int i = 0; i < numOfBindings; ++i) {
        imData->visited[i] = false;
    }

    imData->IMResColumnsForJoin = new uint64_t*[numOfBindings] ;
    imData->IMResColumnsForFilters = new uint64_t*[numOfBindings];
    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {
        imData->IMResColumnsForJoin[i]= nullptr;
        imData->IMResColumnsForFilters[i] = nullptr;
    }
}

void deleteIntermediateData(IMData * imData){
    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {
        delete[] imData->IMResColumnsForJoin[i];
    }
    for (uint64_t i = 0; i < imData->numOfBindings; ++i) {
        delete[] imData->IMResColumnsForFilters[i];
    }
    delete[] imData->visited;
}

void QueryExecutor(RelationMD **Bindings, string *Predicates, string *Projections, int numOfBindings, int numOfPredicates, int numOfProjections) {
    // initialize IM results here, send them below.
    IMData data;
    initializeIMData(&data, numOfBindings);
    int * PParts = nullptr;
    uint64_t * temp;
    for (int i = 0; i < numOfPredicates; ++i) {
        switch(typeOfPredicate(Predicates[i])){
            case 1: //we have a column value compare to number
                //get binded value(pointer to relation) , column value and operator, execute query
                temp = ExecuteNumericalValueQuery(Predicates[i], Bindings, numOfBindings,&data);
                //add to im results
                break;
            case 2:
                //0->binded R1 1-> r1 column || 2->binded r2 3->binded r3
                PParts = getPredicateParts(Predicates[i]);
                //break apart predicate, if is same r exception
                if(PParts[0] == PParts[2]){
                    HandleSameColumnException(PParts, Bindings[PParts[0]], &data);
                }

                //else
                //do the 4 cases: 1 im results of joins are empty.
                    //check if they exist in filters
                    if(data.visited[PParts[0]]){ // if we have a filter
                        //craft the to be sent array by rowId
                    }else{
                        //craft the to bent array by default
                    }
                    if(data.visited[PParts[2]]){ // if we have a filter
                        //craft the to be sent array by rowId
                    }else{
                        //craft the to bent array by default from relation
                    }
                    //case 2
                        //if something exists, but is different from both
                        //get the other pair of values
                        //and cartestiano ginomeno, save in im results, throw the old out!
                    //case 3
                        //if either exist in im res already, for every line in results, find if in the according column the returned value is there,
                        //keep the pleiada, save in new im results, delete old im
                    //case 4
                        // if both exist, for every line in results,
                    delete[] PParts;
                break;
        }
    }

}

uint64_t * HandleSameColumnException(int *PParts, RelationMD *Binding, IMData *imData) {
    //[relid][num of r]
    uint64_t * temp = imData->IMResColumnsForFilters[PParts[0]];
    uint64_t * tempres;
    uint64_t resNum=0;
    if (imData->IMResColumnsForFilters[PParts[0]] != nullptr){
        tempres = new uint64_t[temp[1]];
        for (uint64_t i = 2; i < temp[1]+2; ++i) {
            if(Binding->RelationSerialData[temp[i]*PParts[2]] == Binding->RelationSerialData[temp[i]*PParts[4]]){
                tempres[i-2]=temp[i];
                resNum++;
            }
        }
        delete [] imData->IMResColumnsForFilters[PParts[0]];
    } else{
        tempres = new uint64_t[Binding->RowsNum];
        for (uint64_t i = 0; i < Binding->RowsNum; ++i) {
            if(Binding->RelationSerialData[temp[i]*PParts[2]] == Binding->RelationSerialData[temp[i]*PParts[4]]){
                tempres[i]=temp[i];
                resNum++;
            }
        }
    }
    uint64_t * rvalue = new uint64_t[resNum+2];
    rvalue[0] = PParts[0];
    rvalue[1] = resNum;
    for (int j = 2; j < resNum+2; ++j) {
        rvalue[j] = tempres[j-2];
    }
    delete [] tempres;
    imData->IMResColumnsForFilters[PParts[0]] = rvalue;
    return rvalue;
}

void batchExecutor(List * batch, Data * data){
    ListNode * curr = batch->start;
    string QueryParts[QUERYPARTS];
    RelationMD ** Bindings;
    string * Predicates;
    string * Projections = nullptr;
    int numOfBindings, numOfPredicates, numOfProjections=0;
    while (curr!= nullptr){
        cout <<"Now Processing Query: "<<curr->query<<endl;
        getParts(curr->query, QueryParts);
        Bindings = getBindings(QueryParts[0], data, &numOfBindings); //mia delete xrwstoumenh ana epanalhpsh- delete = clear for next (gia na mh fainontai ta allov del sto loop
        //get2ndpart
        Predicates = getPredicates(QueryParts[1], &numOfPredicates);
        //printStrings(Predicates,getNumOfPredicates(QueryParts[1]));
        //todo get 3rd part - projectionsss
        //execute query
        QueryExecutor(Bindings, Predicates, Projections, numOfBindings, numOfPredicates, numOfProjections);
        curr = curr->next;
        delete [] Bindings;
        delete[] Predicates;
    }
}

void JobExecutor(const string &queriesFile, Data *data) {
    ifstream workload(queriesFile);
    if(!workload){
        cout << "Couldn't open workload file";

    }
    List list;
    string query;
    while (getline(workload, query)){
        //cout << query<< endl;
        //if query is F, send batch to process, reset list
        if(query == "F"){
            //send batch to process
            batchExecutor(&list, data);
            PrintList(&list);
            DeleteList(&list);
            list.start= nullptr;
            list.end = nullptr;
            continue;
        }
        //add query to list
        AddToList(&list, query);
        //PrintList(&list);

    }
    workload.close();

}
