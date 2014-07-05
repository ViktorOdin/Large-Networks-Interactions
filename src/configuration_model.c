/*
 *	Victor Oudin
 *	05/03/2014
 *
 *	LNI: mc.c
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static char * DISTRIB_DEGREE_FILE;	/*** distribution of degree file ***/
static int TWICE_LINKS;			/*** twice the graph number of links ***/
static int * HALF_LINKS_TAB;		/*** table of half-links of the graphe ***/

int twice_links();
int half_links();
void configuration_model();

int main(int argc, char * argv[]){
	int rc;

	/*** main initialisation ***/
	if( argc !=2 ){
		printf("Enter \"./mc distribution_degree_file\"\n");
		printf("Enter \"./mc distribution_degree_file > file.data\" for putting the generated graph in a file\n");\"\n");
		return 1;
	}

	DISTRIB_DEGREE_FILE = argv[1];

	/*** calculation of TWICE_LINKS ***/
	rc = twice_links();
	if( rc != 0 ){
		perror("twice_links() failed\n");
		return 1;
	}

	/*** allocation et remplissage du tableau HALF_LINKS_TAB ***/
	rc = half_links();
	if( rc != 0 ){
		perror("half_links() failed\n");
		return 1;
	}
	
	/*** establishment of graph on std_out ***/
	configuration_model();

	free(HALF_LINKS_TAB);
	return 0;
}

int twice_links(){
	int d, nd;
	FILE* fd = NULL;
	char buff[100];

	fd=fopen(DISTRIB_DEGREE_FILE, "rb");
	if(fd == NULL){
		perror("fopen() DISTRIB_DEGREE_FILE failed\n");
		return 1;
	}

	TWICE_LINKS=0;

	while(fgets (buff, 100, fd)!=NULL){
		if( sscanf(buff,"%d %d\n",&d,&nd) !=2 ){
			perror("sscanf() buff of DISTRIB_DEGREE_FILE scan less than two int\n");
			return 1;
		}
		TWICE_LINKS+=(d*nd);
	}

	fclose(fd);
	return 0;
}

int half_links(){
	int i, j, k, n, d, nd;
	FILE* fd = NULL;
	char buff[100];

	HALF_LINKS_TAB = calloc(TWICE_LINKS,sizeof(int));
	if(HALF_LINKS_TAB==NULL){
		perror("calloc() HALF_LINKS_TAB failed\n");
		printf("calloc() HALF_LINKS_TAB sizeof %d failed\n", TWICE_LINKS)
		return 1;
	}

	fd=fopen(DISTRIB_DEGREE_FILE, "rb");
	if(fd == NULL){
		perror("fopen() DISTRIB_DEGREE_FILE failed\n");
		return 1;
	}

	i=0;n=0;
	while(fgets (buff, 100, fd)!=NULL){
		if( sscanf(buff,"%d %d\n",&d,&nd) !=2 ){
			perror("sscanf() buff of DISTRIB_DEGREE_FILE scan less than two int\n");
			return 1;
		}
		for(j=0; j<nd; j++){
			for(k=0; k<d; k++){
				HALF_LINKS_TAB[i]=n;
				i++;
			}
			n++;
		}
	}
	
	fclose(fd);
	return 0;
}

/*** generation Configuration graph model fonction***/
void configuration_model(){
	int i, u, v, tmp;

	i=TWICE_LINKS;
	srandom(time(NULL));
	while(i>0){
		u = random()%(i);
		tmp = HALF_LINKS_TAB[u];
		HALF_LINKS_TAB[u] = HALF_LINKS_TAB[i-1];
		HALF_LINKS_TAB[i-1] = tmp;
		v = random()%(i-1);
		tmp = HALF_LINKS_TAB[v];
		HALF_LINKS_TAB[v] = HALF_LINKS_TAB[i-2];
		HALF_LINKS_TAB[i-2] = tmp;
		printf("%d %d\n", HALF_LINKS_TAB[i-1], HALF_LINKS_TAB[i-2]);
		i = i-2;
	}
}
