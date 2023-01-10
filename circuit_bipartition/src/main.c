#include "circuit.h"
#include <unistd.h>
#include <time.h>
 
int main(int argc, char *argv[])
{

  	FILE	*fp_bench;
  	NODE	graph[MAX_NODES];

	CELL	noc[MAX_NODES];
	NET	con[MAX_NODES];
	FLIST * flist = NULL;

	
  	LIST	*in=NULL, *out=NULL;
	DLIST **bucket_A=NULL, **bucket_B=NULL;
	int	max_node_id=0;
	int num_node = 0;
	int size_A=0;
	int size_B=0;
	int pmax = 0;
	int curr_cut, i, best_cut, init_cut, passes;
	int flag=0;
	DLIST * max_gain_ptr_A, *max_gain_ptr_B;
	int id = 0;

	clock_t start, end;
	double time;
  
	if(argc!=2)
  	{
     	printf("Insufficient arguments\n");
      	exit(0);
  	}
  
    fp_bench=fopen(argv[1],"r");
    max_node_id = read_circuit(fp_bench, graph, &in, &out);
    print_graph(graph);
		
	init_bipart(graph, noc, &num_node, &size_A, &size_B, max_node_id);
    adj_func( graph, noc, con, max_node_id, &pmax);
	calc_gain(graph, noc, con, max_node_id);
	print_noc(noc, max_node_id, graph);
	print_con(con, max_node_id, graph);
	bucket_list(&bucket_A, &bucket_B, &max_gain_ptr_A, &max_gain_ptr_B, graph, noc, pmax, max_node_id);

	init_cut = cutset(graph, noc, con, max_node_id); //set the initial cut of the first partition
	best_cut = 0;

	printf("\n\t\tMaxNodeID=%d\n", max_node_id);
	printf("\nInitial Cut: %d\n", init_cut);
	printf("\nPress Enter to Start:\n");
	getchar();

	start = clock(); //Time the test

	while(best_cut<=init_cut){// Loop to continue passes until no better cut can be found
		flist = NULL; //Make sure Free list is empty
		for(i=0; i<=max_node_id; i++){
			if(graph[i].type>0){
				//printf("Iteration: %d\t", i);
				update(bucket_A, bucket_B, noc, con, &max_gain_ptr_A, &max_gain_ptr_B, &size_A, &size_B, &id, passes, pmax);
				if(flag == 0){ // Flag for first cut of the pass. Best and current cut will be the same
					best_cut = curr_cut = cutset(graph, noc, con, max_node_id); 
					flag++;
				}
				else
					curr_cut = cutset(graph, noc, con, max_node_id);
				
				insert_flist(&flist, id, curr_cut);
				// Assign the current cut to the best cut if it improves
				if(curr_cut<best_cut) best_cut = curr_cut;				
			}
		}
		//print_bucket_list(bucket_A,bucket_B,pmax);
		//printf("Size A: %d\tSize B: %d\tI: %d\n", size_A, size_B, i);
		printf("Pass: %d\t Cut: %d\n", passes+1, best_cut); 
		//getchar();
		//If the best cut is better than the initial cut, then reset the circuit for the next pass and set new initial cut
		if(best_cut<init_cut){
			init_cut = best_cut;
			reset(graph, noc, con, flist, init_cut, max_node_id);
			calc_gain(graph, noc, con, max_node_id);
			bucket_list(&bucket_A, &bucket_B, &max_gain_ptr_A, &max_gain_ptr_B, graph, noc, pmax, max_node_id);
			passes++;
		}
		flag = 0;
		FLIST * tmp;
		while(flist!=NULL){
			tmp = flist;
			flist = flist->next;
			free(tmp);
		}
		//free(flist);
	}
	end = clock();
	time = (double)(end-start)/CLOCKS_PER_SEC;
	printf("Time: %.3fms\n", time*1000);
	printf("Best Cut: %d\n", init_cut);
	printf("Iterations: %d\n\n", passes);
}
