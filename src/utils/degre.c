/*
 *	Victor Oudin
 *	2/02/2014
 *
 *	degre.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char * DATA_FILE;
static int NBR_NOEUDS;
static int * DEGRE;

int degre();
int write_degre();

int main(int argc, char * argv[]){

	int rc;

	if(argc!=3){ perror("Invalid number of arguments, enter 1 file .data and is numbre of node"); return 1; }
	else{
		DATA_FILE = argv[1];
		NBR_NOEUDS = atoi(argv[2]);
	}

	/*** calcul degre de chaque noeud ***/
	rc = degre();
	if(rc != 0){
		perror("degre failed");
		exit(EXIT_FAILURE);
	}

	/*** ecriture degres ***/
	rc = write_degre();
	if(rc != 0){
		perror("pthread_create failed");
		exit(EXIT_FAILURE);
	}

	free(DEGRE);

	return 0;
}

int degre(){

	FILE* fd = NULL;
	char buff[100];
	int a, b;

	DEGRE= calloc(NBR_NOEUDS,sizeof(int));
	if(DEGRE==NULL){
		perror("calloc failed\n");
		return 1;
	}

	fd=fopen(DATA_FILE, "rb");
	if(fd == NULL){
		perror("\n fopen() failed! \n");
		return 1;
	}

	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&a,&b);
		DEGRE[a]++;DEGRE[b]++;
	}

	fclose(fd);
	return 0;
}

int write_degre(){
	int i;
	FILE* fd = NULL;
	char* ext=".deg";
	char* data_with_ext;
	
	data_with_ext = malloc(strlen(DATA_FILE)+1+4);
	if(data_with_ext==NULL){
		perror("malloc failed\n");
		return 1;
	}
	strcpy(data_with_ext, DATA_FILE);
	strcat(data_with_ext, ext);

	fd=fopen(data_with_ext, "wb");
	if(fd == NULL){
		perror("\n fopen() failed! \n");
		return 1;
	}
	for(i=0;i<NBR_NOEUDS;i++){
		fprintf(fd,"%d\n", DEGRE[i]);
	}
	fclose(fd);
	free(data_with_ext);
	return 0;
}
