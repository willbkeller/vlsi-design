#include "graph.h"
/***************************************************************************************************************************
 Main Function(Serial and Parallel Fault Simulation)
****************************************************************************************************************************/
int main(int argc,char **argv)
{
    FILE *fisc,*fvec,*ffau,*fres;             //file pointers used for .isc file, .vec file, .faults file and resultfile
    int Max,Opt,Npi,Npo,Total,Tfs,i;              //maxnode id,option,tot no of PIs,tot no of Pos,Tot no of input patterns& faults in.vec in.faults

    NODE graph[Mnod];                         //structure used to store the ckt information in .isc file 
    PATTERN vector[Mpt];                      //structure used to store the input vectors information in .vec file 

    //FAULT stuck[Mft];                      //structure used to store the faults information in .faults file
    int a,b,c,d;                             //random variables


    //Read the .isc file and store the information in graph structure
    fisc=fopen(argv[1],"r");                           //file pointer to open .isc file 
    Max=0; 
    Max=ReadIsc(fisc,graph);                 //read .isc file and return index of last node in graph formed
    fclose(fisc);                            //close file pointer for .isc file
    //PrintCircuit(graph,Max);                 //print all members of graph structure

    //Read the .vec file and store the information in  vector structure
    fvec=fopen(argv[2],"r");
    Total=0; //counts the total number of vectors for the .vec file
    Npi=0; // counts the total number of primary inputs for the .vec file
    Total=ReadVec(fvec,vector,&Npi);
    fclose(fvec);
    PrintInputVec(vector,Total,Npi);
    
    Simulation(graph, Max, vector, Total, argv[1]); //runs the simulation for the given graph
    PrintCircuit(graph, Max); //prints the simulated resuts for the circuit


    ClearCircuit(graph,Mnod);                                     //clear memeory for all members of graph
    return 0;
}//end of main
/****************************************************************************************************************************/

