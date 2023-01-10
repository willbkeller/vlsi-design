#include "circuit.h"

/*
	Function to create Cell Array and then Net Array, as well as determine the max number of
	pins on a single cell.
*/
void adj_func(NODE node[MAX_NODES], CELL cell[MAX_NODES], NET net[MAX_NODES], int max_node_id, int * pmax){
	int i,j;
	// Create Cell Array
	for(i=0; i<=max_node_id; i++){
		int count = 0; //to find pmax
		LIST *tmp = node[i].fanin;
		if(node[i].type > 0){
			//Insert all of the fan-in nets to the list of nets on cell[i]
			for(j=0; j<node[i].fanin_num; j++){
				insert_list(&cell[i].nets,tmp->ID);
				tmp = tmp->next;
				count++; //increment count for each fanin pin
			}
			//Insert all of the fan-out nets to the list of nets on cell[i]
			if(node[i].fanout_num>0){
				insert_list(&cell[i].nets, i);
				count++; //increment count for fanout pin if exists
			}
			//If the count of pins on cell[i] is greater than the current pmax value, save count as pmax
			*pmax = count>(*pmax)?count:(*pmax); 
		}
	}
	
	//Create Net Array
	for(i=0; i<=max_node_id; i++){
		LIST *tmp = node[i].fanout;
		if(node[i].type > 0){
			//Insert cell that net is coming out of into list of cells attatched to the net[i]
			insert_list(&net[i].cells, i);
			//Increment the number of cells on net[i] in block A or block B depending on the block of the cell
			if(cell[i].BLK==0)
				net[i].Na++;
			else
				net[i].Nb++;
			
			for(j=0; j<node[i].fanout_num; j++){
				//Insert all fan-out cells connected to net[i] into the list of cells on the net[i]
				insert_list(&net[i].cells,tmp->ID);
				//Increment the number of cells on net[i] in block A or block B depending on the block of the cell
				if(cell[tmp->ID].BLK==0)
					net[i].Na++;
				else
					net[i].Nb++;
				tmp = tmp->next;
			}
		}

	}
}

/*
	Function to print the Cell Array
*/
void print_noc(CELL cell[MAX_NODES], int max_node_id, NODE node[MAX_NODES]){
	int i,j;
	printf("\nCell Array:\nCells\tBlock\tGain\tNets\n");
	for(i=0; i <= max_node_id; i++){
		if(node[i].type>0){
			printf("%2d\t", i);
			printf("%2d\t%2d\t%2d\t", cell[i].BLK, cell[i].gain, cell[i].lock);
			print_list(cell[i].nets);
			printf("\n");
		}
	}
}

/*
	Function to print the Net Array
*/
void print_con(NET net[MAX_NODES], int max_node_id, NODE node[MAX_NODES]){
	int i,j;
	printf("\nNet Array:\nNets\tNa: Nb:\tCells\n");
	for(i=0; i <= max_node_id; i++){
		if(node[i].type>0){
			printf("%d\t%d    %d\t", i, net[i].Na, net[i].Nb);
			print_list(net[i].cells);
			printf("\n");
		}
	}
}

/*
	Function to creat the initial bi-partion of the input circuit
*/
void init_bipart(NODE node[MAX_NODES], CELL cell[MAX_NODES], int * num_node, int* size_A, int * size_B, int max_node_id){
	int i, j;

	// get the number of nodes in the circuit
	for(i=0; i<=max_node_id; i++){
		if(node[i].type>0){
			(*num_node)++;
		}
	}

	for(i=0,j=0; i<=max_node_id; i++){
		if(node[i].type>0){
			//Assign the first half of nodes (+1 if odd number) to block A
			if(j<(*(num_node)/2)+*(num_node)%2){
				cell[i].BLK = 0;
				(*size_A)++;
			}
			//Assign the rest of the nodes to block B
			else{
				cell[i].BLK = 1;
				(*size_B)++;
			}
			j++;
		}
	}
}

/*
	Function to calculate the gain of the bi-parition given its current initial state.
	Modeled from the "Compute Cell Gains" algorithm in Fiduccia-Mattheyses Paper.
*/
void calc_gain(NODE node[MAX_NODES], CELL cell[MAX_NODES], NET net[MAX_NODES], int max_node_id){
	int i;
	int Fn, Tn;
	LIST * cell_p, * net_p;
	for(i=0; i<=max_node_id; i++){
		if(node[i].type>0){
			if(cell[i].lock == 1) cell[i].lock = 0; //Unlock all locked cells
			net_p = cell[i].nets;
			cell[i].gain = 0;
			int FROM = cell[i].BLK; // the From block of cell[i]
			int TO = !FROM;			// the To block of cell[i]
			// Traverse through all nets on the cell
			while(net_p!=NULL){
				//Declare F(n) and T(n)
				if(FROM == 0){
					Fn = net[net_p->ID].Na;
					Tn = net[net_p->ID].Nb;
				}
				else{
					Tn = net[net_p->ID].Na;
					Fn = net[net_p->ID].Nb;
				}
				if(Fn == 1) cell[i].gain++;				
				if(Tn == 0) cell[i].gain--;
				net_p = net_p->next;
			}
		}
	}
}

/*
	Function to generate an initial bucket list for the circuit at the beginning of testing
	or for each new pass.
*/
void bucket_list(DLIST *** bucket_A, DLIST *** bucket_B, DLIST ** max_gain_ptr_A, DLIST ** max_gain_ptr_B, NODE node[MAX_NODES], CELL cell[MAX_NODES], int pmax, int max_node_id){
	int i; int max_gain_A = (-pmax); int max_gain_B = (-pmax);
	//Allocate storage for both buckets of size (pmax*2+1) for [-pmax,+pmax]
	if((*bucket_A)||(*bucket_B)){
		*bucket_A = *bucket_B = NULL;
	}
	*bucket_A = (DLIST**)malloc((pmax*2+1)*sizeof(DLIST*));
	*bucket_B = (DLIST**)malloc((pmax*2+1)*sizeof(DLIST*));
	//create temporary pointer arrays for both buckets
	DLIST **tmp_A = (DLIST**)malloc((pmax*2+1)*sizeof(DLIST*));
	DLIST **tmp_B = (DLIST**)malloc((pmax*2+1)*sizeof(DLIST*));
	for(i=1; i<=max_node_id;i++){
		if(node[i].type>0){
			if(cell[i].lock == 0){
				//Check which bucket cell needs to be assigned to
				//Create Bucket A
				if(cell[i].BLK == 0){
					//Check if bucket[gain] list is empty to initialize it
					if((*bucket_A)[cell[i].gain+pmax]==NULL){
						(*bucket_A)[cell[i].gain+pmax] = (DLIST*)malloc(sizeof(DLIST));
						tmp_A[cell[i].gain+pmax] = (*bucket_A)[cell[i].gain+pmax];
						tmp_A[cell[i].gain+pmax]->ID = i;
						tmp_A[cell[i].gain+pmax]->prev = NULL;
						tmp_A[cell[i].gain+pmax]->next = NULL;
					}
					//If not empty, add next value to list
					else{
						tmp_A[cell[i].gain+pmax]->next = (DLIST*)malloc(sizeof(DLIST));
						tmp_A[cell[i].gain+pmax]->next->ID = i;
						tmp_A[cell[i].gain+pmax]->next->prev = tmp_A[cell[i].gain+pmax];
						tmp_A[cell[i].gain+pmax]=tmp_A[cell[i].gain+pmax]->next;
					}
					if(cell[i].gain>max_gain_A){
						*max_gain_ptr_A = (*bucket_A)[cell[i].gain+pmax];
						max_gain_A = cell[i].gain;
					}

				}
				//Create Bucket B
				else{
					if((*bucket_B)[cell[i].gain+pmax]==NULL){
						(*bucket_B)[cell[i].gain+pmax] = (DLIST*)malloc(sizeof(DLIST));
						tmp_B[cell[i].gain+pmax] = (*bucket_B)[cell[i].gain+pmax];
						tmp_B[cell[i].gain+pmax]->ID = i;
						tmp_B[cell[i].gain+pmax]->prev = NULL;
						tmp_B[cell[i].gain+pmax]->next = NULL;
					}
					else{
						tmp_B[cell[i].gain+pmax]->next = (DLIST*)malloc(sizeof(DLIST));
						tmp_B[cell[i].gain+pmax]->next->ID = i;
						tmp_B[cell[i].gain+pmax]->next->prev = tmp_B[cell[i].gain+pmax];
						tmp_B[cell[i].gain+pmax]=tmp_B[cell[i].gain+pmax]->next;
					}
					if(cell[i].gain>max_gain_B){
						*max_gain_ptr_B = (*bucket_B)[cell[i].gain+pmax];
						max_gain_B = cell[i].gain;
					}
				}
			}
		}	
	}
}

/*
	Function to print both bucket lists
*/
void print_bucket_list(DLIST ** bucket_A, DLIST ** bucket_B, int pmax){	
	int i;
	DLIST ** b_ap = bucket_A;
	DLIST ** b_bp = bucket_B;
	printf("\nBucket List\nGain\tBucket A\tBucket B\n");
	for(i=(pmax*2); i>=0; i--){
		printf("%2d\t", (i-(pmax)));
		if(b_ap[i]==NULL)
			printf("    \t\t");
		else{
			print_dlist(b_ap[i]);
			printf("\t\t");
		}
		if(b_bp[i]==NULL)
			printf("    \n");
		else{
			print_dlist(b_bp[i]);
			printf("\n");
		}
	}
	printf("\n");
}

/*
	Function to generate the cutset for the current state of the partition
*/
int cutset(NODE node[MAX_NODES],CELL cell[MAX_NODES], NET net[MAX_NODES], int max_node_id){
	int cut = 0;
	int i;
	for(i=0; i<=max_node_id; i++){
		if(node[i].type>0){
			//If a net is connected between cells of different paritions, increase the cut
			if(cell[i].BLK==0&&net[i].Nb>0)
				cut++;
			else if(cell[i].BLK==1&&net[i].Na>0)
				cut++;
		}
	}
	return cut;
}

/*
	Function to create and update the Free List in order to retain the best partition
	for each pass.
*/
void insert_flist(FLIST ** head, int ID, int cut)  
{
   FLIST *temp,*tail;
   
   
   if((temp = (FLIST*)malloc(sizeof(FLIST)))==NULL)
   {
     printf("INSER_LIST out of memory\n");
     exit(1);
   }
   
   temp->ID=ID; //Save the ID of the moved cell
   temp->cut = cut; //Save the value of the cut when it was moved
   temp->next=NULL;
   
   if(*head==NULL)
   *head=temp;
   else
   {
     tail=*head;
     while(tail->next!=NULL)
     tail=tail->next;
     tail->next=temp;
   }   
       
}

/*
	Function to update the gain of the parition during a pass.
	Modeled from the "Gain Update" algorithm in Fiduccia-Mattheyses Paper.
*/
void update(DLIST ** bucket_A, DLIST ** bucket_B, CELL cell[MAX_NODES], NET net[MAX_NODES], DLIST ** max_gain_ptr_A, 
				DLIST ** max_gain_ptr_B, int * sizeA, int * sizeB, int * id, int passes, int pmax)
{	
	//if there remain no more elements in the bucket list, escape the function
	//if(*max_gain_ptr_A==NULL||*max_gain_ptr_B==NULL)
		//return;

	int block, Fn, Tn;
	LIST * net_p, * cell_p;
	DLIST * tmp_mxgp;
	DLIST * rm_tmp_mxgp;
	DLIST ** bucket_AP=bucket_A;
	DLIST ** bucket_BP=bucket_B;
	DLIST ** bucket, *tmp, *tmp_2, *base_tmp;
	int pm_offset, i;
	//printf("Size A: %d\tSize B: %d\t", *sizeA, *sizeB);
	// Balance Criteria
	if((*sizeA)>(*sizeB)){
		block = 0;
		(*sizeA)--;
		(*sizeB)++;
	}
	else if((*sizeA)<(*sizeB)){
		block = 1;
		(*sizeB)--;
		(*sizeA)++;
	}
	else{
		block = 0;
		(*sizeA)--;
		(*sizeB)++;
	}

	//assign the correct bucket
	bucket = !block?bucket_A:bucket_B;
	//Assign max gain pointer of the correct bucket
	tmp_mxgp = !block?*max_gain_ptr_A:*max_gain_ptr_B;

	//check if bucket is empty, if so return
	if(tmp_mxgp==NULL) return;
		

	DLIST ** bucket_p;
	*id = tmp_mxgp->ID;

	//Find cell to be moved
	net_p = cell[tmp_mxgp->ID].nets;

	//Initialize From and To block
	int FROM = cell[tmp_mxgp->ID].BLK;
	int TO = !FROM;

	cell[tmp_mxgp->ID].lock = 1; //Lock the Cell
	cell[tmp_mxgp->ID].BLK = !cell[tmp_mxgp->ID].BLK; //Complement its block
	base_tmp = bucket[cell[tmp_mxgp->ID].gain+pmax]; //Pointer to the base cell in the bucket list

	// Remove the base cell from the bucket
	// Empty the location in the bucket list if the base cell is the only one in the location
	if(bucket[cell[tmp_mxgp->ID].gain+pmax]->next==NULL){
		bucket[cell[tmp_mxgp->ID].gain+pmax] = NULL;
		i = 0;
		//Reassign the max gain pointer
		while(bucket[pmax*2-i]==NULL)
			i++;
		tmp_mxgp = bucket[pmax*2-i];
	}
	// Adjust the position of the other cells in the bucket if there are more cells in the location
	else{
		bucket[cell[tmp_mxgp->ID].gain+pmax]=bucket[cell[tmp_mxgp->ID].gain+pmax]->next;		
		bucket[cell[tmp_mxgp->ID].gain+pmax]->prev=NULL;
		tmp_mxgp=bucket[cell[tmp_mxgp->ID].gain+pmax];
	}

	//Assign actual max gain pointers to their new location
	if(block==0) *max_gain_ptr_A=tmp_mxgp;
	else *max_gain_ptr_B=tmp_mxgp;

	//From the "Gain Update" algorithm
	while(net_p!=NULL){
		cell_p = net[net_p->ID].cells;
		if(FROM == 0){
			Fn = net[net_p->ID].Na;
			Tn = net[net_p->ID].Nb;
		}
		else{
			Tn = net[net_p->ID].Na;
			Fn = net[net_p->ID].Nb;
		}
		if(Tn == 0){
			while(cell_p!=NULL){
				if(cell[cell_p->ID].BLK==0)
					bucket_p=bucket_AP;
				else
					bucket_p=bucket_BP;

				if(cell[cell_p->ID].lock == 0){
					cell[cell_p->ID].gain++;
					edit_bucket_list(cell[cell_p->ID].gain, &bucket_p, cell_p->ID, -1, pmax, cell, &tmp_mxgp);
					//update_max_gain_pointer(bucket_A, bucket_B, max_gain_ptr_A, max_gain_ptr_B, pmax);	
				}
				cell_p = cell_p->next;
			}	
		}
		else if(Tn == 1){
			while(cell[cell_p->ID].BLK != TO || cell[cell_p->ID].lock==1){
				if(cell_p->next!=NULL){
					cell_p = cell_p->next;
				}
				else
					break;
			}

			if(cell[cell_p->ID].BLK==0)
				bucket_p=bucket_AP;
			else
				bucket_p=bucket_BP;

			if(cell[cell_p->ID].lock==0&&cell[cell_p->ID].BLK==TO){
				cell[cell_p->ID].gain--;
				edit_bucket_list(cell[cell_p->ID].gain, &bucket_p, cell_p->ID, 1, pmax, cell, &tmp_mxgp);
				//update_max_gain_pointer(bucket_A, bucket_B, max_gain_ptr_A, max_gain_ptr_B, pmax);				
			}
		}

		cell_p = net[net_p->ID].cells;
	
		if(FROM == 0){
			Fn = --net[net_p->ID].Na;
			Tn = ++net[net_p->ID].Nb;
		}
		else{
			Fn = --net[net_p->ID].Nb;
			Tn = ++net[net_p->ID].Na;
		}

		if(Fn == 0){
			while(cell_p!=NULL){
				if(cell[cell_p->ID].BLK==0)
					bucket_p=bucket_AP;
				else
					bucket_p=bucket_BP;
				if(cell[cell_p->ID].lock == 0){
					cell[cell_p->ID].gain--;
					edit_bucket_list(cell[cell_p->ID].gain, &bucket_p, cell_p->ID, 1, pmax, cell, &tmp_mxgp);
					//update_max_gain_pointer(bucket_A, bucket_B, max_gain_ptr_A, max_gain_ptr_B, pmax);
				}
				cell_p = cell_p->next;
			}	
		}
		
		else if(Fn == 1){
			while(cell[cell_p->ID].BLK != FROM){
				if(cell_p->next!=NULL)
					cell_p = cell_p->next;
				else
					break;
			}

			if(cell[cell_p->ID].BLK==0)
				bucket_p=bucket_AP;
			else
				bucket_p=bucket_BP;

			if(cell[cell_p->ID].lock==0){
				cell[cell_p->ID].gain++;
				edit_bucket_list(cell[cell_p->ID].gain, &bucket_p, cell_p->ID, -1, pmax, cell, &tmp_mxgp);
				//update_max_gain_pointer(bucket_A, bucket_B, max_gain_ptr_A, max_gain_ptr_B, pmax);
			}
		}
		cell_p = net[net_p->ID].cells;
		net_p = net_p->next;
	}
	update_max_gain_pointer(bucket_A, bucket_B, max_gain_ptr_A, max_gain_ptr_B, pmax);

	//free(base_tmp);

}

/*
	Function to reset the partition for the next pass based on the best cut that was
	encountered during the previous pass.
*/
void reset(NODE node[MAX_NODES], CELL cell[MAX_NODES], NET net[MAX_NODES], FLIST * flist, int best_cut, int max_node_id){
	int i, FROM, TO;
	NET * np = net;
	LIST * cp;
	FLIST * fl = flist;
	
	//Traverse the free list to find the cell that was moved to create the best cut
	while(fl->cut!=best_cut){
		fl = fl->next;
	}
	//Move to the next cell if it is available
	if(fl->next!=NULL)
		fl = fl->next;
	
	/*
		Complement the block of all cells from that point forward to reinstate the partition with the 
		best cut for the pass
	*/
	while(fl!=NULL){
		cell[fl->ID].BLK = !cell[fl->ID].BLK;
		fl = fl->next;
	}

	//Reset and Recompute N(A) and N(B) for all nets in the circuit
	for(i = 0; i<max_node_id; i++){
		if(node[i].type>0){
			np[i].Na = np[i].Nb = 0;
			cp = np[i].cells;
			while(cp != NULL){
				FROM = cell[cp->ID].BLK;
				TO = !FROM;
				if(FROM == 0){
					if(cell[cp->ID].BLK == FROM)
						np[i].Na++;
					else
						np[i].Nb++;
				}
				else
					if(cell[cp->ID].BLK == FROM)
						np[i].Nb++;
					else
						np[i].Na++;
				cp = cp->next;
			}
		}
	}
}

/*
	Function to print the Free List
*/
void printflist(FLIST * flist){
	FLIST * flp = flist;
	int size = 0;
	while(flp!=NULL){
		printf("ID: %d, CUT: %d;\t", flp->ID, flp->cut);
		size++;
		flp = flp->next;
	}
	printf("%d\n", size);
}

/*
	Function to remove a cell from its current location in the bucket
	list so that it can be placed in its new location
*/
void remove_BList(int gain, DLIST *** bucket_p, DLIST ** tmp_b, int ID, int flag, int pmax){
	if((*tmp_b)->prev==NULL){ //Check if cell is at the front of the list
		if((*tmp_b)->next!=NULL){ //Check if it is the only cell in the list
			(*tmp_b)->next->prev=NULL;
			(*bucket_p)[gain+flag+pmax]=(*tmp_b)->next;
		}
		else
			(*bucket_p)[gain+flag+pmax]=NULL;
	}
	else{
		(*tmp_b)->prev->next = (*tmp_b)->next;
		if((*tmp_b)->next!=NULL)
			(*tmp_b)->next->prev = (*tmp_b)->prev;
	}

}

/*
	Function to insert removed cell to its new location in the bucket
*/
void insert_BList(int gain, DLIST *** bucket_p, DLIST ** tmp_b, DLIST ** tmp, int ID, int pmax){
	if((*tmp_b)==NULL){ //Check if list is empty
			(*bucket_p)[gain+pmax]=(*tmp);
			(*bucket_p)[gain+pmax]->next=NULL;
			(*bucket_p)[gain+pmax]->prev=NULL;
		}
		else{ //Place cell at the end of the list
			while((*tmp_b)->next!=NULL)
				(*tmp_b)=(*tmp_b)->next;
			(*tmp)->prev=(*tmp_b);
			(*tmp)->next=NULL;
			(*tmp_b)->next=(*tmp);	
		}
}

/*
	Function to update the bucked list after base cell has been moved.

	Variable flag is the opposite of the gain change of the cell so that the 
	bucket list the cell is currently in will be accessed in order to move it
	to its next location.
*/
void edit_bucket_list(int gain, DLIST *** bucket_p, int ID, int flag, int pmax, CELL cell[MAX_NODES], DLIST ** max_gain_ptr){
	DLIST * tmp, *tmp_2;
	if(cell[ID].lock!=1){ // Confirm cell is not locked
		tmp = (*bucket_p)[gain+flag+pmax]; //Pointer to the pre-gain update list
		tmp_2 = (*bucket_p)[gain+pmax];    //Pointer to the new-gain list
		//Find cell in the bucket list
		while(tmp->ID!=ID){
			tmp = tmp->next;
		}
		remove_BList(gain, bucket_p, &tmp, ID, flag, pmax);
		insert_BList(gain, bucket_p, &tmp_2, &tmp, ID, pmax); 
	}
}

void update_max_gain_pointer(DLIST ** bucket_A, DLIST ** bucket_B, DLIST ** max_gain_ptr_A, DLIST ** max_gain_ptr_B, int pmax){
	int i=0;
	//Updating max gain pointer location for bucket A
	while(bucket_A[(pmax*2)-i]==NULL){
		i++;
		if(i>(pmax*2)){ //out of bounds error check
			break;
		}
	}
	if(i<=(pmax*2))
		(*max_gain_ptr_A) = bucket_A[(pmax*2)-i];
	else
		(*max_gain_ptr_A) = NULL;

	i=0;
	//Updating max gain pointer location for bucket B
	while(bucket_B[(pmax*2)-i]==NULL){
		i++;
		if(i>(pmax*2)){ //out of bounds error check
			break;
		}
	}
	if(i<=(pmax*2))
		(*max_gain_ptr_B) = bucket_B[(pmax*2)-i];
	else
		(*max_gain_ptr_B) = NULL;
}
