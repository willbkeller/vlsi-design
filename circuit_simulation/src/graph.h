/***************************************************************************************************************************
Header Files
****************************************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include <assert.h>
#include <limits.h>
/***************************************************************************************************************************
Constant Declarations 
****************************************************************************************************************************/
// VARIOUS CONSTANTS
#define Mfnam      20			// max size for a file name
#define Mnod    15000 		        // max number of nodes in a graph/node
#define Mlin      300			// max size of characters in a line
#define Mnam       25			// max size of a node name
#define Mtyp       10			// max type of nodes/gates
#define Mout       16		        // max node out degree (Nfo)
#define Min         9			// max node in degree (Nfi)
#define Mpi       233			// max number of primary inputs
#define Mpo       140			// max number of primary outputs
#define Mpt       10			// max number of input patterns in .vec file
#define Mft       10			// max number of stuck at faults in .faults file
// NODE TYPE CONSTANTS 
#define INPT 1				// Primary Input
#define AND  2				// AND 
#define NAND 3				// NAND 
#define OR   4				// OR 
#define NOR  5				// NOR 
#define XOR  6				// XOR 
#define XNOR 7				// XNOR 
#define BUFF 8				// BUFFER 
#define NOT  9				// INVERTER 
#define FROM 10				// BRANCH

#define DC 2 //Don't Care
/***************************************************************************************************************************
Structure Declarations 
****************************************************************************************************************************/
//1.Stucture declaration for LIST
typedef struct LIST_type {
   int  id;		   //id for current element		
   struct LIST_type *next;  //pointer to next id element( if there is no element, then it will be NULL)		
} LIST;
//2.Stucture declaration for NODE
typedef struct NODE_type
{
  char Name[Mnam];                      //name of the node
  int Type,Nfi,Nfo,Po,Value;                  //type, nooffanins, nooffanouts,primaryo/p
  int Mark,Cval,Fval;                    //marker,correctvalue,faultvalue
  LIST *Fin,*Fot;                      //fanin members, fanout members 
} NODE;
//3.Stucture declaration for PATTERN
typedef struct PATTER_type{
  int vect[Mpi]; // array to store primary inputs for current vector file
}PATTERN;

/***************************************************************************************************************************
LIST Structure Functions
****************************************************************************************************************************/
void InsertList(LIST **,int);
void PrintList(LIST *);
void FreeList(LIST **);
/***************************************************************************************************************************
 NODE Structure Functions
****************************************************************************************************************************/
int ReadIsc(FILE *,NODE *);
void InitializeCircuit(NODE *,int);
int AssignType(char *);
void PrintCircuit(NODE *,int);
void ClearCircuit(NODE *,int);
/***************************************************************************************************************************
 PATTERN Structure Functions
****************************************************************************************************************************/

/***************************************************************************************************************************
User Defined Functions in user.c
****************************************************************************************************************************/
int ReadVec(FILE*,PATTERN*,int*);
void PrintInputVec(PATTERN*,int, int);
void Simulation(NODE*, int, PATTERN*, int, char[]);
void GateCalculation(NODE*, int);
void GetFin(LIST*, int[]);
/****************************************************************************************************************************/
