/*
 *	Victor Oudin
 *	5/02/2014
 *
 *	cc-uf.c
 */
#include "utils.h"

static char * DATA_FILE;			/*** fichier .data ***/
static char * DEGRE_FILE;			/*** fichier .deg ***/
static int NBR_NOEUDS;				/*** nombre de noeuds du graphe ***/ 
static int SIZE_1CC, SIZE_2CC, SIZE_3CC;	/*** taille des 3 plus grandes composantes connexes ***/
static int * RACINES;				/*** tableau de racines des composantes connexes ***/
static int * RANG;				/*** tableau du rang des racines des composantes connexes ***/

inline int max(int a, int b){
	return a > b ? a : b;
}

int main(int argc, char * argv[]){
	int rc;
	/*** initialisation du main ***/
	if(argc!=4){
		printf("Entrez \"./cc-uf <graph> <size of graph> <deg file of graph>\"\n");
		return 1;
	}

	DATA_FILE = argv[1];
	NBR_NOEUDS = atoi(argv[2]);
	DEGRE_FILE= argv[3];

	if(NBR_NOEUDS<1){
		perror("Arguments invalides, nombre de noeuds invalide\n");
		return 1;
	}

	/*** comptage des composantes connexes ***/
	rc = cc();
	if(rc < 0){
		perror("compconnexes failed");
		return 1;
	}

	/*** affichage résultats ***/
	double fraction = ((double)SIZE_1CC/(double)NBR_NOEUDS);
	printf("nombre de composantes connexes : %d\n", rc);
	printf("taille de la 1er composante connexe : %d\n", SIZE_1CC);
	printf("taille de la 2nd composante connexe : %d\n", SIZE_2CC);
	printf("taille de la 3eme composante connexe : %d\n", SIZE_3CC);
	printf("fraction de la 1er composante connexe : %f\n", fraction);
	
	return 0;
}

/*** cc() compte le nombre de composantes connexes et le retourne ***/
/*** retourne -1 en cas d'erreur ***/
int cc(){
	FILE* fd = NULL;		// descripteur de fichier
	char buff[100];			// buffer de lecture
	int n1, n2;			// noeuds lu
	int nbr_compcon=0;		// nombre de composantes connexes
	int racine_1cc, racine_2cc, racine_3cc; // racines des 3 plus grandes composantes connexes
	int * size;			// tableau des tailles des composantes connexes
	int i;

	/*** allocation mémoire du tableau de racines des composantes connexes ***/
	RACINES = calloc(NBR_NOEUDS, sizeof(int));
	if(RACINES == NULL){
		perror("calloc failed\n");
		return -1;
	}
	/*** initialisation du tableau de racines ***/
	for(i=0; i<NBR_NOEUDS; i++){
		RACINES[i]=i;
	}
	/*** allocation mémoire du tableau du rang des racines des composantes connexes ***/
	RANG = calloc(NBR_NOEUDS, sizeof(int));
	if(RANG == NULL){
		perror("calloc failed\n");
		return -1;
	}
	/*** ouverture du fichier .data ***/
	fd=fopen(DATA_FILE, "rb");
	if(fd == NULL){
		perror("fopen() failed! \n");
		return -1;
	}
	/*** lecture du fichier .data ***/
	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&n1, &n2);
		/*** union des ensembles de n1 et n2 ***/
		union_cc(&RANG, &RACINES, n1, n2);
	}
	fclose(fd);
	/*** finalisation du tableau des racines ***/
	for(i=0; i<NBR_NOEUDS; i++){
		RACINES[i]=find(&RACINES, i);
	}

	racine_1cc=0;
	racine_2cc=0;
	racine_3cc=0;

	/*** allocation mémoire du tableau des tailles des composantes connexes ***/
	size = calloc(NBR_NOEUDS, sizeof(int));
	if(size == NULL){
		perror("calloc failed\n");
		return -1;
	}

	/*** calcul de la taille des composantes connexes ***/
	for(i=0; i<NBR_NOEUDS; i++){
		if(size[RACINES[i]]==0){
			nbr_compcon++;		// nouvelle composante connexe
		}
		size[RACINES[i]]++;		// augmentation de la taille de la composante connexe courante
		if(size[RACINES[i]]>size[racine_1cc] 
			&& RACINES[i]!=racine_1cc){
			/*** cas la composante connexe courante est plus grande que la 1er ***/
			racine_3cc=racine_2cc;
			racine_2cc=racine_1cc;
			racine_1cc=RACINES[i];
		}
		else if(size[RACINES[i]]>size[racine_2cc] 
			&& RACINES[i]!=racine_2cc 
			&& RACINES[i]!=racine_1cc){
			/*** cas la composante connexe courante est plus grande que la 2nd ***/
			racine_3cc=racine_2cc;
			racine_2cc=RACINES[i];
		}
		else if(size[RACINES[i]]>size[racine_3cc] 
			&& RACINES[i]!=racine_3cc 
			&& RACINES[i]!=racine_2cc 
			&& RACINES[i]!=racine_1cc){
			/*** cas la composante connexe courante est plus grande que la 3eme ***/
			racine_3cc=RACINES[i];
		}
	}

	/*** enregistrement des tailles des composantes connexes dans les variables static ***/
	SIZE_3CC=size[racine_3cc];
	SIZE_2CC=size[racine_2cc];
	SIZE_1CC=size[racine_1cc];
	
	free(size);
	free(RACINES);
	free(RANG);

	return nbr_compcon;
}
