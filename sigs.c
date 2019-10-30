/*signature creation
June 18th - July 30th 2019 
Svetlana Sodol 
UBC
*/


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>


/******* FG-MPI Boilerplate begin *********/
#include "fgmpi.h"
int my_main( int argc, char** argv ); /*forward declaration*/
FG_ProcessPtr_t binding_func(int argc, char** argv, int rank){
return (&my_main);
}
FG_MapPtr_t map_lookup(int argc, char** argv, char* str){
return (&binding_func);
}
int main( int argc, char *argv[] )
{
FGmpiexec(&argc, &argv, &map_lookup);
return (0);
}
/******* FG-MPI Boilerplate end *********/



typedef struct Node  { 
 	int data; 
 	struct Node *next; 
} Node; 

//TODO make size dynamically allocated
//function that collects array into one character buffer and prints
void print_array(int a[], int a_size, char prt[], int rank) {

	char rnk[20];
	sprintf(rnk, "%d", rank);

	char buffer[200];
	strcpy(buffer, prt);
	strcat(buffer, rnk);
	char spc[20] = " ";
	strcat(buffer, spc);
	char del[20] = ": ";
	strcat(buffer, del);
 
    for(int i=0; i< a_size;i++){
		char elem[20];
		sprintf(elem, "%d", a[i]);
		strcat(buffer, elem);
		strcat(buffer, spc);
	}   // for

	strcat(buffer, "\n\n");
	printf("%s",buffer);


}//print



//allocates hash-set pairs to workers

//waits for all sets to finish
//finds outlier set
//finds similar sets based on signatures
//finds closest set to last set in data file

//sends quit command
void manager_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){

	//threshold for overlaps
	int t = num_hash/2;

	int data;
	//worker node rank
	int worker;
	//to hold the hash-set pair ranks
	int pair[2];

	int candidates = 0;

	//TODO figure out a way to stagger set-hash assigntment

	for (int i = 0; i < num_hash; i++){
		for (int j = 0; j < num_sets; j++){

		//receive note that worker is free
		//worker sends its rank to indicate that it is available
		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		worker = data;

		//assign this pair i j to worker
		//send as tuple
		pair[0]= num_hash + j; //set first
		pair[1]= num_hash + num_sets + i; //hash second

		//printf("Assigning set %d with hash %d to worker %d\n", pair[0], pair[1], worker);

		MPI_Send(pair, 2, MPI_INT, worker, 0, MPI_COMM_WORLD);

		}// for set

	}//for hash


	//receives message from workers that they are ready but shut them down instead
	//printf("Shutting down workers\n");
	for (int i = 0; i< num_worker; i++){
		//printf("i is %d\n", i);
		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("worker %d\n", data);
		//size+1 is used as indicator for shutting down
		pair[0]= size+1; 
		pair[1]= size+1;
		MPI_Send(&pair, 2, MPI_INT, data, 0, MPI_COMM_WORLD);
		//printf("messege sent\n");
			
	}//for
	//printf("Workers shut down\n");

	//begin candidate pairs and outlier processing

	int st_set = num_sets*num_sets;

	int *set1;
	set1 = (int *)malloc(sizeof(int)*st_set);

	int *set2;
	set2 = (int *)malloc(sizeof(int)*st_set);

	int *count;
	count = (int *)malloc(sizeof(int)*st_set);

	for (int i = 0; i < st_set; i++){
		set1[i] = 0;
	}

	for (int i = 0; i < st_set; i++){
		set2[i] = 0;
	}

	for (int i = 0; i < st_set; i++){
		count[i] = 0;
	}

	//malloc 3 arrays of size num_sets*num_sets
	//same index indicates candidate pair 
	//last array stores number of matches/clashes between the two sets
	
	//stores number of edges for each set
	//each edge is a minhash match to another set
	int * match;
	match = (int *)calloc(num_sets, sizeof(int));

	//process signatures as they come in
	//create linked list for each hash to store all matches on that hash
	for (int i = 0; i <num_hash; i++){

		Node ** minh = calloc(num_elem, sizeof(struct Node*));

		for (int i = 0; i < num_elem; i++){
			minh[i] = NULL;
		}
		
		//rank of signature (hash)
		int dest = i;
		//send something to synchronize
		int data = 2;
		
		MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	


		for (int j = 0; j < num_sets; j++){

			MPI_Recv(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (data == num_elem+1){continue;}//if empty set

			Node *new_node = malloc( sizeof( struct Node ) );
			new_node->data = j;
			new_node->next = minh[data];
			minh[data] = new_node;

			Node * cur = minh[data];

				while (cur->next != NULL){
					//printf("%d\n ", cur->data);
					
					//printf("Sets %d and %d overlap on minhash %d\n", cur->next->data, j, i);
					
					//store this info into the tree for outlier
					match[cur->next->data] = match[cur->next->data]+1;
					match[j] = match[j]+1;
					

					cur = cur->next;

					for (int i = 0; i < st_set; i++){
						if ((set1[i]==cur->data)&&(set2[i]==j)){
							count[i]++;
							break;
						}
						if ((set1[i]==j)&&(set2[i]==cur->data)){
							count[i]++;
							break;
						}

						if ((set1[i]==0)&&(set2[i]==0)){
							set1[i]=cur->data;
							set2[i]=j;
							count[i]=1;
							candidates++;
							break;
						}
				}//for updating count
			}//while clashes 
		}//for set
	}//for hash
	
	char text[] = "Matches ";
	//print_array(match, num_sets, text, rank);
	
	//find the outlier
	int index=0;
	int min = num_sets*num_hash;
	for (int i=0; i<num_sets; i++){
		if (match[i]<min){
			index=i;
			min = match[i];
		}//if
	
	}//for sets to find min
	
	printf("The outlier is set %d with %d edges\n", index, min);

	//find candidate pairs
	for (int i = 0; i < st_set; i++){

		if (count[i] >= t){

		//printf("Candidate pair %d and %d with count %d\n\n", set1[i], set2[i], count[i]);}

	}//for printing candidate pairs

	//sends ready to collect clashes command
	int cmd[2];
	cmd[0] = size+1;
	cmd[1] = size+1;
	printf("Starting similarity calculation...\n");
	
	int * clash;
	clash = (int *)calloc(num_sets-1, sizeof(int));
	
	//printf("ready to collect clashes\n");
	int cnt = 0;
	int set;
	
	while(cnt < num_hash){
		MPI_Recv(&set, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (set>size){
			cnt++;
			//printf("New signature is done %d\n", cnt);
		}else {
			clash[set] = clash[set]+1;
			//printf("Collected new clash\n");
		}//else
	
	}//while
	
	char prt[] = "Clashes ";
	//print_array(clash, num_sets-1, prt, rank);
	
	int best = num_sets-1;
	int max = 0;
	
	for (int i = 0; i < num_sets-1; i++){
		if (clash[i]> max){
			max = clash[i];
			best = i;
		}//if
	}//for
		
	printf("Best match is set %d with overlap %d\n", best, max);
	
	//allocate 2 sets
	int * st1;
	st1 = (int *)malloc(sizeof(int)*num_elem);
	
	int * st2;
	st2 = (int *)malloc(sizeof(int)*num_elem);
	
	//sets	
	for (int i = 0; i<num_sets-1; i++){
		int dest = num_hash +i;
		if(i==best){
			data = 1;
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}//if
		else{
			data = 0;
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}
	
	}//for
	
	//get best match set
	for (int j =0; j <num_elem; j++){
		MPI_Recv(&data, 1, MPI_INT, num_hash+best, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		st1[j]=data;
	}//for
	
	char prt1[] = "Best Match ";
	//print_array(st1, num_elem, prt1, rank);
	
	//get query set
	data = 1;
	MPI_Send(&data, 1, MPI_INT, num_hash+num_sets-1, 0, MPI_COMM_WORLD);
	for (int j =0; j <num_elem; j++){
		MPI_Recv(&data, 1, MPI_INT, num_hash+num_sets-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		st2[j]=data;
	}//for
	
	char prt2[] = "Query Set ";
	//print_array(st2, num_elem, prt2, rank);
	
	int act = 0;
	int un = 0;
	for (int i = 0; i < num_elem; i++){
		if (st2[i]==1){
			un = un+1;
			if(st1[i]==1){
				act = act+1;	
			}//if
		}else if (st1[i]==1){
			un = un+1;
		}//else
	}//for
	
	float aprx = (float) max/num_hash;
	printf("Approximate Jaccard similarity calculated by minhash is %f\n", aprx);
	float actual = (float) act/un;
	printf("Actual Jaccard similarity is %f\n", actual);
}//manager



//reads off and stores a set 
//sends off the set when prompted
void set_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){
	
	//set is a binary string - 1 if element of that index is present in the set
	//size of set is num_elements

	//printf("I am set %d\n", rank);

	int data; 

	//malloc a set of size num_elem
	int * st;
	st = (int *)malloc(sizeof(int)*num_elem);

	for (int i=0; i<num_elem; i++){
		MPI_Recv(&data, 1, MPI_INT, size-2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		st[i] = data;
	}

	char prt[] = "Set ";
	//print_array(st, num_elem, prt, rank);

	//send our set to whoever needs
	for (int i = 0; i<num_hash; i++){

		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int dest = data;
		//printf("Sending set %d to worker %d\n", rank, dest);
		//if (dest>size){break;}
		for (int j =0; j <num_elem; j++){
			data = st[j];
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}//for 
	} //for

	MPI_Recv(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (data==1){
		for (int j =0; j <num_elem; j++){
			data = st[j];
			MPI_Send(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
		}//for 
	}//if
	
	free(st);

}//set



//calls manager with its rank to get assignment
//receives set and hash to follow to create signature
//calls set to get a copy of the set
//calls hash to get the order of elements to check
//creates signature for that hash-set pair by visiting elements in the hash order
//gives signature to manager when done
void worker_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size){

	//printf("I am worker %d\n", rank);

	int data; 
	int dest;

	int pair[2];

	//malloc a set of size num_elem
	int * st;
	st = (int *)malloc(sizeof(int)*num_elem);

	//malloc hash array

	int *hash;
	hash = (int *)malloc(sizeof(int)*size_hash);

	//size of signature is num_hash

	int sig;

	
	while(1){

		//call manager with our rank
	
		MPI_Send(&rank, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);

		// get set-hash pair from manager
		MPI_Recv(&pair, 2, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//get the ordering from the hash
		dest = pair[1];
		
		if (dest>size){
			//printf("Manager shut me down %d\n", rank);
			break;
		}
		//printf("Calling hash %d from worker %d\n", dest, rank);
		MPI_Send(&rank, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

		//printf("Message sent %d\n", rank);

		for (int j =0; j <size_hash; j++){
			MPI_Recv(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			hash[j]=data;
		}//for
		//printf("Worker %d received hash %d: %d %d %d %d\n", rank, dest, hash[0], hash[1], hash[2], hash[3]); 

		//get the set
		dest = pair[0];
		//printf("Calling set %d from worker %d\n", dest, rank);
		MPI_Send(&rank, 1, MPI_INT, pair[0], 0, MPI_COMM_WORLD);

		//printf("Message sent %d\n", rank);

		for (int j =0; j <num_elem; j++){
			MPI_Recv(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			st[j]=data;
		}//for

		//printf("Worker %d received set %d: %d %d %d %d\n", rank, dest, st[0], st[1], st[2], st[3]); 

		for (int j =0; j <size_hash; j++){
			dest = hash[j];
		
			int sign[2];

			if (st[dest]==1){
				sig=dest;
				//printf("Set %d found first 1\n", pair[0]);
				//printf("Worker %d has signature %d for set %d and hash %d\n", rank, sig, pair[0], pair[1]);

				sign[0] = pair[0];
				sign[1] = sig;
				//tag 1 for signature sending
				MPI_Send(sign, 2, MPI_INT, pair[1]-num_hash-num_sets, 1, MPI_COMM_WORLD);
				break;
			} else if (j == size_hash-1) {

				sig=num_elem+1;
				//printf("Set %d does not have signature for hash %d\n", pair[0], pair[1]);
				sign[0] = pair[0];
				sign[1] = sig;
				//tag 1 for signature sending
				MPI_Send(sign, 2, MPI_INT, pair[1]-num_hash-num_sets, 1, MPI_COMM_WORLD);
			}//else if

		}//for hash length
		
		
	}//while

	free(st);
	free(hash);

}//worker


//stores signature for a specific hash
//works with manager to first send all in order to create graph
//then calculates own clashes with the last query set
void signature_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){
	
	int *sign;
	sign = (int *)malloc(sizeof(int)*num_sets);

	int data[2];
	
	for(int i = 0; i < num_sets; i++){
		MPI_Recv(&data, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if (data[0]>size){
			//printf("Manager shut me down %d\n", rank);
			break;
			      }
		sign[data[0]-num_hash] = data[1];
		//printf("Hash %d received signature %d for set %d\n", rank, data[1], data[0]);
		
	}//for
	
	char prt[] = "Signature for hash ";
	//print_array(sign, num_sets, prt, rank);
	
	MPI_Recv(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	for (int i = 0; i < num_sets; i++){
		//send to manager
			MPI_Send(&sign[i], 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
		}//for
	
	//printf("Sending clashes\n");

	for (int i = 0; i < num_sets-1; i++){
		if(sign[num_sets-1]>num_elem){break;}
		if(sign[i]>num_elem){continue;}
		if(sign[i]==sign[num_sets-1]){
		//printf("Query set clashes with set %d on hash %d\n", i, rank);
		//send to manager
		MPI_Send(&i, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
		}//if
	}//for
	//send done command to manager
	int done = size+1;
	//printf("Signature %d is done\n", rank);
	MPI_Send(&done, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);

	free(sign);
}//signature

//creates random ordering of elements
//provides the ordering when prompted
void hash_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){

	//printf("I am hash %d\n", rank);

	int dest, data;

	srand(time(0)+rank);

	int *hash;
	hash = (int *)malloc(sizeof(int)*size_hash);

	int * elem;
	elem = (int *)malloc(sizeof(int)*num_elem);

	for (int i = 0; i < num_elem; i++) elem[i] = i;

	hash[0] = size-3-num_worker - rank;
	elem[size-3-rank - num_worker] = num_elem+1;

	// Random permutation the order
	for (int i = 1; i < size_hash; i++) {
		int j;

		do {

			j = rand() % num_elem;

		} while(elem[j]==num_elem+1);

		hash[i] = elem[j];
		elem[j] = num_elem+1;
	}

	char prt[] = "Hash ";
	//print_array(hash, size_hash, prt, rank);

	for(int i = 0; i<num_sets; i++){

		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		dest = data;
		//printf("Sending hash %d to set %d\n", rank, dest);
		if (dest>size){
			//printf("Manager shut me down %d\n", rank);
			break;
		}
		for (int j =0; j <size_hash; j++){
			data = hash[j];
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}//for 
	} //while
	free(hash);
	free(elem);

}//hash


//reads in the file
//sends the read in sets to the set nodes
void reader_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){
	//malloc a set
	int * st;
	st = (int *)malloc(sizeof(int)*num_elem);
	int data;
	//open file for reading
	
	FILE *fp;
	fp = fopen("data-valid-200-61188.txt", "r");

	for (int i=0; i<num_sets; i++){
		for(int j = 0; j < num_elem; j++){
			
			data = getc(fp);
			//send character to set node i
			
			data = data - 48;

			MPI_Send(&data, 1, MPI_INT, num_hash+i, 0, MPI_COMM_WORLD);

			//read in new character
		}//while
		data = getc(fp);
	}//for

	fclose(fp);

}//reader

//main initializes and assigns roles
int my_main(int argc, char ** argv){

	int rank, size;
	int num_elem, num_sets, num_hash, size_hash;
	int last_elem, last_set, last_hash, num_worker;

	//initialize
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	num_elem = atoi(argv[1]);
	num_sets = atoi(argv[2]);
	num_hash = atoi(argv[3]);
	size_hash = atoi(argv[4]);


	last_elem = num_hash - 1;
	last_set = num_hash + num_sets - 1;
	last_hash = num_hash + num_sets + num_hash -1;
	num_worker = size - 3 - last_hash;
	
	//printf("We have %d workers\n", num_worker);
	

	//printf("I am %d of %d\n", rank, size);

	//assign roles
	if (rank == size-1){manager_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank == size-2){reader_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank <= last_elem){signature_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank <= last_set){set_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank <= last_hash){hash_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else {worker_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	
	//clean up
	//printf("rank %d done\n", rank);
	MPI_Finalize();
	return 0;
}


/*
MPI_Send(
    void* data,
    int count,
    MPI_Datatype datatype,
    int destination,
    int tag,
    MPI_Comm communicator)
MPI_Recv(
    void* data,
    int count,
    MPI_Datatype datatype,
    int source,
    int tag,
    MPI_Comm communicator,
    MPI_Status* status)
*/


/* to compile
mpicc sigc.c -o sigs
to run 
mpiexec -n  ./sigs num_elem num_sets num_hash size_hash (rest are workers)
with fgmpi (4 workers):
mpiexec -nfg 1 -n 22 ./sigs 5 8 4 4 
OR
mpiexec -nfg 22 -n 1 ./sigs 5 8 4 4
*/
