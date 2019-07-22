/*signature creation
June 18th 2019 
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

//make size dynamically allocated
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
//finds similar sets based on signatures

//sends quit command
void manager_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){

	//threshold for overlaps
	int t = 2;

	int data;
	int worker;
	int pair[2];

	int candidates = 0;

	//figure out a way to stagger set-hash assigntment

	for (int i = 0; i < num_hash; i++){
		for (int j = 0; j < num_sets; j++){

		//receive note that worker is free
		//worker sends its rank to indicate that it is available
		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		worker = data;

		//assign this pair i j to worker
		//send as tuple
		pair[0]= num_elem + j; //set first
		pair[1]= num_elem + num_sets + i; //hash second

		printf("Assigning set %d with hash %d to worker %d\n", pair[0], pair[1], worker);

		MPI_Send(pair, 2, MPI_INT, worker, 0, MPI_COMM_WORLD);

		}// for set

	}//for hash


	//receives message from workers that they are ready but shut them down instead
	for (int i = 0; i< num_worker; i++){
		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		pair[0]= size+1; 
		pair[1]= size+1;
		MPI_Send(&pair, 2, MPI_INT, data, 0, MPI_COMM_WORLD);
			
	}//for

/*
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

	//malloc 1 array of size num_elem 
	//elem keeps current hashes clashes - stores rank of set that has that hash

	//malloc 3 arrays of size num_sets
	//same index indicates candidate pair 
	//last array stores number of clashes

	for (int i = 0; i <num_hash; i++){

		Node ** minh = calloc(num_elem, sizeof(struct Node*));

		for (int i = 0; i < num_elem; i++){
			minh[i] = NULL;
		}


		for (int j = 0; j < num_sets; j++){
			//rank of set
			int dest = num_elem+j;
			//index of current minhash
			int data = i;
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

			MPI_Recv(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (data == num_elem+1){break;}//if empty set

			Node *new_node = malloc( sizeof( struct Node ) );
			new_node->data = dest;
			new_node->next = minh[data];
			minh[data] = new_node;

			Node * cur = minh[data];

				while (cur->next != NULL){
					//printf("%d\n ", cur->data);
					
					//printf("Sets %d and %d overlap on minhash %d\n", cur->next->data, dest, i);

					cur = cur->next;

					for (int i = 0; i < st_set; i++){
						if ((set1[i]==cur->data)&&(set2[i]==dest)){
							count[i]++;
							break;
						}
						if ((set1[i]==dest)&&(set2[i]==cur->data)){
							count[i]++;
							break;
						}

						if ((set1[i]==0)&&(set2[i]==0)){
							set1[i]=cur->data;
							set2[i]=dest;
							count[i]=1;
							candidates++;
							break;
						}
				}//for updating count
			}//while clashes 
		}//for set
	}//for hash

	for (int i = 0; i < st_set; i++){

		if (count[i] >= t){

		printf("Candidate pair %d and %d with count %d\n\n", set1[i], set2[i], count[i]);}

	}//for printing candidate pairs

	data = size+1;

*/
	//sends quit command
	data = size+1;
	printf("Shutting down...\n");
	
	//elements
	for (int i = 0; i<num_elem; i++){
		MPI_Send(&data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}//for
		
	//hashes	
	for (int i = 0; i<num_hash; i++){
		int dest = num_elem + num_sets - 1 +i;
		MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	
	}//for

	//make the manager somehow find and report candidate pairs based on minhash signatures
}//manager



//reads off and stores a set 
//sends off the set when prompted
void set_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){
	//change the following to read in a set
	//set is a binary string - 1 if element of that index is present in the set
	//size of set is num_elements

	//printf("I am set %d\n", rank);

	int data; 

	//malloc a set of size num_elem
	int * st;
	st = (int *)malloc(sizeof(int)*num_elem);

	srand(time(0)+rank);

	for (int i=0; i<num_elem; i++){
		st[i] = rand() % 2;
	}

	char prt[] = "Set ";
	print_array(st, num_elem, prt, rank);


	//send our set to whoever needs
	for (int i = 0; i<num_hash; i++){

		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int dest = data;
		printf("Sending set %d to worker %d\n", rank, dest);
		//if (dest>size){break;}
		for (int j =0; j <num_elem; j++){
			data = st[j];
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}//for 

		//receive the signature back and store it 
	} //for

	//send signature to manager


	//send message to manager that we are done
	//data = 1;
	//MPI_Send(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);

	free(st);

}//set



//calls manager with its rank to get assignment
//receives set and hash to follow to create signature
//calls set to get a copy of the set
//calls hash to get the order of elements to check
//creates signature for that hash-set pair by visiting elements in the hash order
//gives signature to manager when done?
void worker_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size){
	//change the following to read in a set
	//set is a binary string - 1 if element of that index is present in the set
	//size of set is num_elements

	//printf("I am set %d\n", rank);

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
	//malloc signature array

	//	int *sig;
	//	sig = (int *)malloc(sizeof(int)*num_hash);

	int sig;

	
	while(1){

		//call manager with our rank
	
		MPI_Send(&rank, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);

		// get set-hash pair from manager
		MPI_Recv(&pair, 2, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//get the ordering from the hash
		dest = pair[1];
		
		if (dest>size){break;}
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
			//printf("Worker %d calling element %d\n", rank, dest);
			MPI_Send(&rank, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

			MPI_Recv(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			//printf("Set %d checking presence of element %d: %d\n", s, data, st[data]);

			if (st[data]==1){
				sig=data;
				//printf("Set %d found first 1\n", s);
				continue;
			} else if (j == size_hash-1) {

				sig=num_elem+1;
			}//else if

			printf("Worker %d has signature %d for set %d and hash %d\n",rank, sig, pair[0], pair[1]);

		}//for hash length
		
		
	}//while

	//printf("Set %d sending signature to manager\n", rank);
/*
	char str[] = "Signature for set ";
	print_array(sig, num_hash, str, rank);

	//dont need to let manager know we are done
	data = 1;
	MPI_Send(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
	//int dest, data;

	while(1){
		MPI_Recv(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (data>size){break;}
		data =sig[data];
		MPI_Send(&data, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);
	}//while
*/
	free(st);
	//free(sig);
	free(hash);

}//worker


//represents a possible element of data
//reads and stores that element
//when prompted gives the element 
//stores index of set if that element is present in it
void element_fn(int rank, int num_elem, int num_sets, int size_hash, int num_hash, int num_worker, int size){
	//the element is the index
	//make it read an element
	printf("I am element %d\n", rank);
	
	int element = rank;
	int dest, data;

	while(1){
		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		dest = data;
		if (dest>size){break;}
		//printf("Sending element %d to set %d\n", rank, dest);
		MPI_Send(&element, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	}//while
}//element

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

	hash[0] = size-2-num_worker - rank;
	elem[size-2-rank - num_worker] = num_elem+1;

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
	print_array(hash, size_hash, prt, rank);

	while(1){

		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		dest = data;
		//printf("Sending hash %d to set %d\n", rank, dest);
		if (dest>size){break;}
		for (int j =0; j <size_hash; j++){
			data = hash[j];
			MPI_Send(&data, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
		}//for 
	} //while
	free(hash);
	free(elem);

}//hash


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


	last_elem = num_elem - 1;
	last_set = num_elem + num_sets - 1;
	last_hash = num_elem + num_sets + num_hash -1;
	num_worker = size - 2 - last_hash;
	

	//printf("I am %d of %d\n", rank, size);

	//assign roles
	if (rank == size-1){manager_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank <= last_elem){element_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank <= last_set){set_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else if (rank <= last_hash){hash_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	else {worker_fn(rank, num_elem, num_sets, size_hash, num_hash, num_worker, size);}
	
	//clean up
	printf("rank %d done\n", rank);
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

with fgmpi (2 workers):
mpiexec -nfg 1 -n 18 ./sigs 4 4 4 4 
OR
mpiexec -nfg 18 -n 1 ./sigs 4 4 4 4

*/
