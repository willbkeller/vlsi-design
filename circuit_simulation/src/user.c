#include "graph.h"
#include <string.h>

// Logic for Truth Tables
int AND_truth[3][3]={{0,0,0},
                     {0,1,DC},
                     {0,DC,DC}};

int OR_truth[3][3]={{0,1,DC},
                    {1,1,1},
                    {DC,1,DC}};

int XOR_truth[3][3]={{0,1,DC},
                     {1,0,DC},
                     {DC,DC,DC}}; 

int INV_truth[3]={1,0,DC};

int FROM_truth[3]={0,1,DC};

// Reads the Vector file and store each vector in the Pattern struct, also gets the NPI for each vector
// and returns the total number of vectors
int ReadVec(FILE* fvec,PATTERN* vector, int* Npi){
  char line[Mlin];
  bzero(line,strlen(line));
  int novec = 0; // number of vector counter initialized
  int i;
  while(!feof(fvec)){
    fgets(line,Mlin,fvec);
    *Npi = strlen(line)-1; // gets the npi of each line for the current vector
    for(i=0; i<*Npi; i++){
      if(line[i] == '0'|| line[i] == '1'){ vector[novec].vect[i] = line[i] - '0';} // stores the integer value in the vector by subtracting ASCII values
      else{vector[novec].vect[i] = DC;} 
    }    
    novec++;

  }
  return novec-1; 
}

// prints each input vector value to terminal
void PrintInputVec(PATTERN* vector,int Total, int Npi){
  int i,j;
  printf("Input Vectors (X = 2)\n");
  for(i=0; i<Total; i++){
    for(j=0; j<Npi; j++){
      printf("%d", vector[i].vect[j]);
    }
    printf("\n");
  }
}

// Simulation for given graph
void Simulation(NODE* graph, int Max, PATTERN* vector, int Total, char file[]){
  int final[Total][Mpo]; // initalize 2D array to store the outputs for each vector pattern
  int pi_count; // counts number of primary inputs for file writing
  int po_count; // counts number of primary outputs for file writing
  for(int j=0; j<Total; j++){ // loops through each vector
    pi_count = 0;
    po_count = 0;
    for(int i=0; i<=Max; i++){
      if(graph[i].Type != 0){
        //apply input patterns
        if(graph[i].Type == INPT){
            graph[i].Cval = vector[j].vect[pi_count];
            pi_count++;
        }
        else{
          GateCalculation(graph, i); // if not an input, calculates the gate value by passing in the graph and the current id number
        }
      }
      // adds the Cval to the final output array if current node has primary output flag
      if(graph[i].Po == 1){
        final[j][po_count] = graph[i].Cval;
        po_count++;
      }
    }
  }
  strcat(file,"_result.txt"); // create filename from command line argument
  FILE* newFile = fopen(file, "w"); //creates a result file
  // loop to write a line for each input/output vector in the .vec file
  for(int j = 0; j<Total; j++){
    for(int i=0; i<pi_count; i++){ // loop to write input vector to current file line
      if(vector[j].vect[i] == DC) 
        fprintf(newFile, "x");
      else
        fprintf(newFile, "%d", vector[j].vect[i]);
    }
    fprintf(newFile, "\t");
    for(int i=0; i<po_count; i++){ // loop to write input vector to current file line
      if(final[j][i] == DC) 
        fprintf(newFile, "x");
      else
        fprintf(newFile, "%d", final[j][i]);
    }
    fprintf(newFile,"\n");
  }
}

// calculates the Cval for the current node (same pattern for each gate type)
void GateCalculation(NODE* graph, int i){
  // AND Gate
  if(graph[i].Type == AND){
    int vect[graph[i].Nfi]; // creates an array to store the ids for each fan in with size of Nfi for current node
    GetFin(graph[i].Fin, vect); 
    int logicVal = AND_truth[graph[vect[0]].Cval][graph[vect[1]].Cval]; // gets current logic value for first two gate inputs
    if(graph[i].Nfi>2){
      for(int j=2; j<(graph[i].Nfi); j++){ // if there are more than two fan ins, updates current logic value ith the next input
        logicVal = AND_truth[logicVal][graph[vect[j]].Cval];
      }
    }
    graph[i].Cval = logicVal; // writes the final logic value to the current nodes Cval
  }
  //NAND Gate
  else if(graph[i].Type == NAND){
    int vect[graph[i].Nfi];
    GetFin(graph[i].Fin, vect);
    int logicVal = AND_truth[graph[vect[0]].Cval][graph[vect[1]].Cval];
    if(graph[i].Nfi>2){
      for(int j=2; j<(graph[i].Nfi); j++){
        logicVal = AND_truth[logicVal][graph[vect[j]].Cval];
      }
    }
    graph[i].Cval = INV_truth[logicVal]; // inverts the final logic value 
  }
  // OR Gate
  else if(graph[i].Type == OR){
    int vect[graph[i].Nfi];
    GetFin(graph[i].Fin, vect);
    int logicVal = OR_truth[graph[vect[0]].Cval][graph[vect[1]].Cval];
    if(graph[i].Nfi>2){
      for(int j=2; j<(graph[i].Nfi); j++){
        logicVal = OR_truth[logicVal][graph[vect[j]].Cval];
      }
      graph[i].Cval = logicVal;
    }
    else{
      graph[i].Cval = logicVal;
    }
  }
  // NOR Gate
  else if(graph[i].Type == NOR){
    int vect[graph[i].Nfi];
    GetFin(graph[i].Fin, vect);
    int logicVal = OR_truth[graph[vect[0]].Cval][graph[vect[1]].Cval];
    if(graph[i].Nfi>2){
      for(int j=2; j<(graph[i].Nfi); j++){
        logicVal = OR_truth[logicVal][graph[vect[j]].Cval];
      }
    }
    graph[i].Cval = INV_truth[logicVal];  
  }
  // XOR Gate
  else if(graph[i].Type == XOR){
    int vect[graph[i].Nfi];
    GetFin(graph[i].Fin, vect);
    int logicVal = XOR_truth[graph[vect[0]].Cval][graph[vect[1]].Cval];
    if(graph[i].Nfi>2){
      for(int j=2; j<(graph[i].Nfi); j++){
        logicVal = XOR_truth[logicVal][graph[vect[j]].Cval];
      }
      graph[i].Cval = logicVal;
    }
    else{
      graph[i].Cval = logicVal;
    }
  }
  // XNOR Gate
  else if(graph[i].Type == XNOR){
    int vect[graph[i].Nfi];
    GetFin(graph[i].Fin, vect);
    int logicVal = XOR_truth[graph[vect[0]].Cval][graph[vect[1]].Cval];
    if(graph[i].Nfi>2){
      for(int j=2; j<(graph[i].Nfi); j++){
        logicVal = XOR_truth[logicVal][graph[vect[j]].Cval];
      }
      graph[i].Cval = INV_truth[logicVal];
    }
    else{
      graph[i].Cval = INV_truth[logicVal];
    }
  }

  //BUFFER
  else if(graph[i].Type == BUFF){
    graph[i].Cval = graph[graph[i].Fin->id].Cval; // Passes value from Fan in
  }
  //NOT Gate
  else if(graph[i].Type == NOT){
    graph[i].Cval = INV_truth[graph[graph[i].Fin->id].Cval]; // Inverts value from fan in
  }
  //FROM (Branch)
  else if(graph[i].Type == FROM){
    graph[i].Cval = FROM_truth[graph[graph[i].Fin->id].Cval]; // Passes value from fan in
  }
  else{printf("ERROR: Invalid Type");} // prints error if current gate is an invalid type
}

// traverses the linked list Fin and store each Fin id into an array
void GetFin(LIST* Fin, int vect[]){
  int i=0;
  LIST *tmp = NULL;
  tmp = Fin;
  while(tmp != NULL){
    vect[i]=tmp->id;
    tmp = tmp->next;
    i++;
  }
  return;
}