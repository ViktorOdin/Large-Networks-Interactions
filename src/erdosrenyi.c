/*
 *	Victor Oudin
 *	05/03/2014
 *
 *	LNI: er.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int NODES;		/*** graph number of nodes ***/
static int LINKS;		/*** graph number of links ***/

int erdosrenyi();

int main(int argc, char * argv[]){
	int rc;

	/*** main initialisation ***/
	if( argc !=3 ){
		printf("Enter \"./er number_of_nodes number_of_links\"\n");
		printf("Enter \"./er number_of_nodes number_of_links > file.data\" for putting the generated graph in a file\n");
		return 1;
	}

	NODES = atoi(argv[1]);
	LINKS = atoi(argv[2]);

	/*** establishment of graph on std_out ***/
	rc = erdosrenyi();
	if( rc != 0 ){
		perror("erdosrenyi() failed\n");
		return 1;
	}

	return 0;
}

/*** generation graph of Erdos-Renyi fonction ***/
int erdosrenyi(){
	int n1, n2, nbr_liens=0;

	srandom(time(NULL));
	while( nbr_liens < LINKS ){
		n1 = random()%NODES;
		n2 = random()%NODES;
		if( n1 != n2 ){
			printf("%d %d\n", n1, n2);
			nbr_liens++;
		}
	}

	return 0;
}
