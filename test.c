

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 int main(int argc, char** argv){
  int num_elem = 4;
  int num_sets=4;
	//malloc a set
	int * st;
	st = (int *)malloc(sizeof(int)*num_elem);

	//const char filename[] = "example.txt";
	//open file for reading
	FILE *fp;
	fp = fopen("example.txt", "r");

	int data;

	for (int i=0; i<num_sets; i++){
		for(int j = 0; j < num_elem; j++){
			
			data = getc(fp);
			//send character to set node i
			
			data = data - 48;

			printf("read in new number %d\n", data);

			//read in new character
		}//while
		printf("\n");
		data = getc(fp);
	}//for

	fclose(fp);
  
  }//main
