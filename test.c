

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 int main(int argc, char** argv){
  int num_elem = 4;
  int num_sets=4;
	//malloc a set
	int * st;
	st = (int *)malloc(sizeof(int)*num_elem);

	//open file for reading
	FILE *fp;
	fp = fopen('example.txt', 'r');

	char ch;

	for (int i=0; i<num_sets; i++){

		ch = getc(fp);
		while((ch!='\n')){
			//send character to set node i

			//turn character to int
			data = atoi(ch);

			printf("read in new number %d:\n", data);

			//read in new character
			ch = getc(fp);
		}//while
	}//for

	fclose(fp);
  
  }//main
