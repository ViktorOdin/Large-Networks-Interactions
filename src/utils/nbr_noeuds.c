/*
 *	Victor Oudin
 *	2/02/2014
 *
 *	nbr_noeuds.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char * DATA_FILE;
static int NBR_NOEUDS;

inline int max(int a, int b, int c){
	return a>b&&a>c ? a : b>a&&b>c ? b : c;
}

int nbr_noeuds();
int write_nbr_noeud();

int main(int argc, char * argv[]){

	int rc;

	if(argc!=2){ perror("Invalid number of arguments, enter one file .data"); return 1; }
	else{
		DATA_FILE = argv[1];
	}

	/*** nombre de noeuds ***/
	rc = nbr_noeuds();
	if(rc != 0){
		perror("nbr_noeuds failed");
		exit(EXIT_FAILURE);
	}

	/*** ecriture nbr noeuds ***/
	rc = write_nbr_noeud();
	if(rc != 0){
		perror("pthread_create failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int nbr_noeuds(){

	FILE* fd = NULL;
	char buff[100];
	int nbr_noeuds=0, a, b;;

	fd=fopen(DATA_FILE, "rb");
	if(fd == NULL){
		perror("fopen() failed\n");
		return 1;
	}

	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&a,&b);
		nbr_noeuds=max(nbr_noeuds,a,b);
	}

	fclose(fd);
	NBR_NOEUDS = nbr_noeuds+1;
	return 0;
}

int write_nbr_noeud(){

	FILE* fd = NULL;
	char* ext=".n";
	char* data_with_ext;
	
	data_with_ext = malloc(strlen(DATA_FILE)+1+2);
	if(data_with_ext==NULL){
		perror("malloc failed\n");
		return 1;
	}
	strcpy(data_with_ext, DATA_FILE);
	strcat(data_with_ext, ext);

	fd=fopen(data_with_ext, "wb");
	if(fd == NULL){
		perror("fopen() failed\n");
		return 1;
	}
	fprintf(fd,"%d\n", NBR_NOEUDS);
	fclose(fd);

	free(data_with_ext);
	return 0;
}
