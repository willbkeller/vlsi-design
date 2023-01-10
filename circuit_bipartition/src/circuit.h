#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>

 /* NODE TYPE CONSTANTS */
#define INPT	1				/* Primary Input*/
#define AND   	2				/* AND       */
#define NAND 	3				/* NAND      */
#define OR  	4				/* OR        */
#define NOR 	5				/* NOR       */
#define NOT  	6				/* INVERTER  */
#define OUTPUT  7				/* OUTPUT    */ 
#define BUFF    8				/* BUFFER    */

#define MAX_NODES   25000

typedef struct list
{
   int ID;
   struct list *next;
} LIST;

typedef struct dlist{
  int ID;
  struct dlist *next;
  struct dlist *prev;
} DLIST;

typedef struct flist{
  int ID;
  int cut;
  struct flist *next;
}FLIST;

typedef struct node
{
  int type;			//Gate Type
  int fanout_num;	//Number of fan-outs from a gate
  int fanin_num;	// Number of fan-ins to a gate
  int output;
  LIST *fanin;		//pointer to Linked list which represent the Fan-ins to a gate.
  LIST *fanout;	//pointer to Linked list which represent the Fan-outs from a gate.
 
} NODE;

/*-----------------------------------------------------------------
                  New Structures
------------------------------------------------------------------*/   

typedef struct cell
{
	LIST *nets;
	int BLK;
  int gain;
  int lock;
  
}CELL;

typedef struct net
{
	LIST *cells;
	int Nb,Na;

}NET;








/*---------------------------------------------------------------------------
 			Function Prototypes                                                       
---------------------------------------------------------------------------*/

int read_circuit(FILE*,NODE[MAX_NODES],LIST**,LIST**);
void insert_list(LIST **, int);  
void initialize_graph(NODE *); 
void charcat(char * ,char );  
int assign_type(char *);
void print_graph(NODE [MAX_NODES]);
void print_list(LIST *);
void print_dlist(DLIST *);
int count_list(LIST *); 




/*-----------------------------------------------------------------
                  New Function Prototype
------------------------------------------------------------------*/   

void adj_func( NODE[MAX_NODES], CELL[MAX_NODES], NET[MAX_NODES], int, int*); //create net and cell array
void print_noc(CELL [MAX_NODES], int, NODE[MAX_NODES]); //print nets of cells : cell array
void print_con(NET [MAX_NODES], int, NODE[MAX_NODES]); //
void init_bipart(NODE[MAX_NODES], CELL[MAX_NODES], int*, int*, int*, int);
void print_bipart(NODE[MAX_NODES], CELL[MAX_NODES], int);
void calc_gain(NODE[MAX_NODES], CELL[MAX_NODES], NET[MAX_NODES], int);
void bucket_list(DLIST***, DLIST***, DLIST **, DLIST **, NODE[MAX_NODES], CELL[MAX_NODES], int, int);
void print_bucket_list(DLIST**, DLIST**, int);
int cutset(NODE[MAX_NODES],CELL[MAX_NODES],NET[MAX_NODES],int);
void insert_flist(FLIST **, int, int);
void update(DLIST **, DLIST **, CELL[MAX_NODES], NET[MAX_NODES], DLIST**,DLIST**, int*, int*, int*, int, int);
void reset(NODE[MAX_NODES], CELL[MAX_NODES], NET[MAX_NODES], FLIST *, int, int);
void edit_bucket_list(int, DLIST ***, int, int, int, CELL[MAX_NODES], DLIST ** max_gain_ptr);
void printflist(FLIST *);
void insert_BList(int, DLIST***, DLIST **, DLIST **, int, int);
void remove_Blist(int, DLIST ***, DLIST**, int, int, int);
void update_max_gain_pointer(DLIST **, DLIST **, DLIST **, DLIST **, int);



















 

