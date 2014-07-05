/*
 *	Victor Oudin
 *	05/03/2014
 *
 *	mc.c
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static char * DISTRIB_DEGRE_FILE;
//static int NBR_NOEUDS;		/*** nombre de noeuds du graphe ***/
static int NBR2_LIENS;			/*** 2 fois le nombre de liens du graphe ***/
static int * DEMIS_LIENS;		/*** tableau des demis liens du graphe ***/

int nbr2_liens();
int demis_liens();
void modconfig();

int main(int argc, char * argv[]){
	int rc;

	/*** initialisation du main ***/
	if( argc !=2 ){
		printf("Entrez \"./mc <fichier de distribution des degres\"\n");
		return 1;
	}

	DISTRIB_DEGRE_FILE = argv[1];

	/*** calcul de 2NBR_LIENS ***/
	rc = nbr2_liens();
	if( rc != 0 ){
		perror("2nbr_liens failed\n");
		return 1;
	}

	/*** allocation et remplissage du tableau DEMIS_LIENS ***/
	rc = demis_liens();
	if( rc != 0 ){
		perror("demis_liens failed\n");
		return 1;
	}
	
	/*** modélisation du graphe sur sortie standart ***/
	modconfig();

	free(DEMIS_LIENS);
	return 0;
}

int nbr2_liens(){
	int d, nd;
	FILE* fd = NULL;
	char buff[100];

	fd=fopen(DISTRIB_DEGRE_FILE, "rb");
	if(fd == NULL){
		perror("fopen failed\n");
		return 1;
	}

	NBR2_LIENS=0;

	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&d,&nd);
		NBR2_LIENS+=(d*nd);
	}

	fclose(fd);
	return 0;
}

int demis_liens(){
	int i, j, k, n, d, nd;
	FILE* fd = NULL;
	char buff[100];

	DEMIS_LIENS = calloc(NBR2_LIENS,sizeof(int));
	if(DEMIS_LIENS==NULL){
		perror("calloc failed\n");
		return 1;
	}

	fd=fopen(DISTRIB_DEGRE_FILE, "rb");
	if(fd == NULL){
		perror("fopen failed\n");
		return 1;
	}

	i=0;n=0;
	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&d,&nd);
		for(j=0; j<nd; j++){
			for(k=0; k<d; k++){
				DEMIS_LIENS[i]=n;
				i++;
			}
			n++;
		}
	}
	
	fclose(fd);
	return 0;
}

void modconfig(){
	int i, u, v, tmp;

	i=NBR2_LIENS;
	srandom(time(NULL));
	while(i>0){
		u = random()%(i);
		tmp = DEMIS_LIENS[u];
		DEMIS_LIENS[u] = DEMIS_LIENS[i-1];
		DEMIS_LIENS[i-1] = tmp;
		v = random()%(i-1);
		tmp = DEMIS_LIENS[v];
		DEMIS_LIENS[v] = DEMIS_LIENS[i-2];
		DEMIS_LIENS[i-2] = tmp;
		printf("%d %d\n", DEMIS_LIENS[i-1], DEMIS_LIENS[i-2]);
		i = i-2;
	}
}
