/*
 *	Victor Oudin
 *	05/03/2014
 *
 *	er.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int NBR_NOEUDS;		/*** nombre de noeuds du graphe ***/
static int NBR_LIENS;		/*** nombre de liens du graphe ***/

int erdosrenyi();

int main(int argc, char * argv[]){
	int rc;

	/*** initialisation du main ***/
	if( argc !=3 ){
		printf("Entrez \"./er <nombre de noeuds> <nombre de liens>\"\n");
		return 1;
	}

	NBR_NOEUDS = atoi(argv[1]);
	NBR_LIENS = atoi(argv[2]);

	/*** modélisation du graphe sur sortie standart ***/
	rc = erdosrenyi();
	if( rc != 0 ){
		perror("erdosrenyi failed\n");
		return 1;
	}

	return 0;
}

int erdosrenyi(){
	int n1, n2, nbr_liens=0;

	srandom(time(NULL));
	while( nbr_liens < NBR_LIENS ){
		n1 = random()%NBR_NOEUDS;
		n2 = random()%NBR_NOEUDS;
		if( n1 != n2 ){
			printf("%d %d\n", n1, n2);
			nbr_liens++;
		}
	}

	return 0;
}
