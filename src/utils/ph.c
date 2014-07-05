/*
 *	Victor Oudin
 *	12/02/2014
 *
 *	ph.c
 */
#include "utils.h"
#include "ensemble.h"

static char * DATA_FILE;			/*** fichier .data ***/
static char * DEGRE_FILE;			/*** fichier .deg ***/
static int NBR_NOEUDS;				/*** nombre de noeuds du graphe ***/ 
static int RACINE_CC, SIZE_CC;			/*** racine et taille de la plus grande composante connexe ***/
static int SIZE_PH;				/*** taille de peripherie de la plus grande composante connexe ***/
static int * RACINES;				/*** tableau de racines des composantes connexes ***/
static int * RANG;				/*** tableau du rang des racines des composantes connexes ***/
static int * DEGRE;				/*** tableau des degre de chaque noeud ***/
static int * DEGRETMP;				/*** copie de DEGRE ***/
static int * PERIPH;				/*** tableau de peripherie de la plus grande composante connexe ***/
static int ** TAB_VOISINNAGE;			/*** tableau de voisinage du graphe ***/

inline int max(int a, int b){
	return a > b ? a : b;
}

int main(int argc, char * argv[]){
	int i, rc, ph;
	double result;
	/*** initialisation du main ***/
	if(argc!=5){
		printf("Entrez \"./ph <graph> <size of graph> <deg file of graph> <deg>\"\n");
		return 1;
	}

	DATA_FILE = argv[1];
	NBR_NOEUDS = atoi(argv[2]);
	DEGRE_FILE= argv[3];
	ph = atoi(argv[4]);

	if(NBR_NOEUDS<1){
		perror("Arguments invalides, nombre de noeuds invalide\n");
		return 1;
	}

	/*** lecture du fichier de degre ***/
	rc = read_degre(DEGRE_FILE,&DEGRE,NBR_NOEUDS);
	if(rc != 0){
		perror("degre failed");
		return 1;
	}

	/*** fabrication du tableau de voisinage ***/
	rc = tab_voisinnage(DATA_FILE, &TAB_VOISINNAGE, &DEGRE, NBR_NOEUDS);
	if(rc != 0){
		perror("tab_voisinnage failed");
		return 1;
	}
	
	/*** fraction de la peripherie de la plus grande composante connexe ***/
	rc = premierecc();
	if(rc != 0){
		perror("premierecc failed");
		return 1;
	}

	PERIPH = calloc(NBR_NOEUDS,sizeof(int));
	if(PERIPH==NULL){
		perror("calloc failed\n");
		return 1;
	}
	int * distrib = calloc(NBR_NOEUDS,sizeof(int));
	if(distrib==NULL){
		perror("calloc failed\n");
		return 1;
	}

	/*** periphérie de 1 à ph ***/
	int k=1;
	while(k<=ph){
		rc = periph(k);
		if(rc != 0){
			perror("periph failed");
			return 1;
		}
		result=((double)SIZE_PH)/((double)SIZE_CC);
		printf("taille de la plus grande composante connexe : %d\n", SIZE_CC);
		printf("taille de la peripherie %d de la plus grande composante connexe : %d\n", k, SIZE_PH);
		printf("fraction de la peripherie %d de la plus grande composante connexe %f\n", k, result);

		for(i=0; i<NBR_NOEUDS; i++){
			if(PERIPH[i]){
				distrib[DEGRE[i]]++;
			}
		}
		printf("distribution des degres dans la peripherie %d\n", k);
		for(i=0; i<NBR_NOEUDS; i++){
			if(distrib[i]>0){
				printf("°%d\t%d noeuds\n", i, distrib[i]);
			}
		}
		k++;
		printf("\n");
	}
	free(distrib);

	for(i=0; i<NBR_NOEUDS; i++){
		free(TAB_VOISINNAGE[i]);
	}
	free(TAB_VOISINNAGE);
	free(DEGRE);
	free(RACINES);
	free(RANG);
	free(PERIPH);

	return 0;
}

/*** premierecc() enregistre la taille de la plus grande composante connexe dans la variable static SIZE_CC ***/
/*** retourne 0 ***/
/*** retourne 1 en cas d'erreur ***/
int premierecc(){
	FILE* fd = NULL;		// descripteur de fichier
	char buff[100];			// buffer de lecture
	int n1, n2;			// noeuds lu
	int i;
	int * size;

	/*** allocation mémoire du tableau de racines des composantes connexes ***/
	RACINES = calloc(NBR_NOEUDS, sizeof(int));
	if(RACINES == NULL){
		perror("calloc failed\n");
		return 1;
	}
	/*** initialisation du tableau de racines ***/
	for(i=0; i<NBR_NOEUDS; i++){
		RACINES[i]=i;
	}
	/*** allocation mémoire du tableau du rang des racines des composantes connexes ***/
	RANG = calloc(NBR_NOEUDS, sizeof(int));
	if(RANG == NULL){
		perror("calloc failed\n");
		return 1;
	}
	/*** ouverture du fichier .data ***/
	fd=fopen(DATA_FILE, "rb");
	if(fd == NULL){
		perror("fopen() failed! \n");
		return 1;
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

	RACINE_CC=0;

	/*** allocation mémoire du tableau des tailles des composantes connexes ***/
	size = calloc(NBR_NOEUDS, sizeof(int));
	if(size == NULL){
		perror("calloc failed\n");
		return 1;
	}

	/*** calcul de la taille de la plus grande composante connexe ***/
	for(i=0; i<NBR_NOEUDS; i++){
		size[RACINES[i]]++;		// augmentation de la taille de la composante connexe courante
		if(size[RACINES[i]]>size[RACINE_CC] 
			&& RACINES[i]!=RACINE_CC){
			/*** cas la composante connexe courante est plus grande celle enregistrée ***/
			RACINE_CC=RACINES[i];
		}
	}

	/*** enregistrement de la taille de la plus grande composante connexe dans la variable static ***/
	SIZE_CC=size[RACINE_CC];

	free(size);
	return 0;
}

/*** periph2() enregistre la taille de la peripherie 2 
 * de la plus grande composante connexe dans la variable static SIZE_PH ***/
/*** retourne 0 ***/
/*** retourne 1 en cas d'erreur ***/
int periph2(){
	int i, rc;
	int nc, ncv;		// noeud courant et voisins du noeud courant

	struct ensemble *ens1, *ens2;	// ensembles des noeuds de degre 1 et 2 de la plus grande composante connexe

	/*** allocation des ensembles ***/
	ens1=(struct ensemble *)new_ensemble(NBR_NOEUDS);
	if(ens1 == NULL){ perror("new_ensemble failed\n"); return 1; }

	ens2=(struct ensemble *)new_ensemble(NBR_NOEUDS);
	if(ens2 == NULL){ perror("new_ensemble failed\n"); return 1; }

	/*** initialisation des ensembles ***/
	for(i=0; i<NBR_NOEUDS; i++){
		if(RACINES[i]==RACINE_CC && DEGRE[i]==1){
			rc = add(ens1, &i);
			if(rc < 0){ perror("add failed\n"); return 1; }
		}
		else if(RACINES[i]==RACINE_CC && DEGRE[i]==2){
			rc = add(ens2, &i);
			if(rc < 0){ perror("add failed\n"); return 1; }
		}
	}

	/*** copie du tableau de degre dans DEGRETMP ***/
	DEGRETMP = calloc(NBR_NOEUDS,sizeof(int));
	if(DEGRETMP==NULL){ perror("calloc failed\n"); return 1; }
	for(i=0; i<NBR_NOEUDS; i++){
		DEGRETMP[i]=DEGRE[i];
	}

	SIZE_PH=0;		// initialisation de la taille de la peripherie
	/*** enlevement de tous les noeuds de degre 1 de la plus grande composante connexe ***/
	while(!isEmpty(ens1)){
		nc=get(ens1);
		rc = del(ens1, &nc);
		if(rc != 0){ perror("del failed\n"); return 1; }
		SIZE_PH++;	// augmentation de la taille de la peripherie
		PERIPH[nc]=1;	// enregistrement du noeud courant dans le tableau de peripherie
		/*** parcours des voisins du noeud courant ***/
		for(i=0; i<DEGRE[nc]; i++){
			ncv=TAB_VOISINNAGE[nc][i];
			DEGRETMP[ncv]--;	// reduction du degre des voisins du noeud courant
			/*** cas ou leurs nouveau degre est égale à 1 ***/
			if(DEGRETMP[ncv]==1){
				rc = add(ens1, &ncv);
				if(rc < 0){ perror("add failed\n"); return 1; }
				rc = del(ens2, &ncv);
				if(rc < 0){ perror("del failed\n"); return 1; }
			}
			/*** cas ou leurs nouveau degre est égale à 2 ***/
			else if(DEGRETMP[ncv]==2){
				rc = add(ens2, &ncv);
				if(rc < 0){ perror("add failed\n"); return 1; }
			}
		}
	}
	/*** enlevement de tous les noeuds de degre 2 de la plus grande composante connexe ***/
	while(!isEmpty(ens2)){
		nc=get(ens2);
		rc = del(ens2, &nc);
		if(rc != 0){ perror("del failed\n"); return 1; }
		SIZE_PH++;	// augmentation de la taille de la peripherie
		PERIPH[nc]=1;	// enregistrement du noeud courant dans le tableau de peripherie
		for(i=0; i<DEGRE[nc]; i++){
			ncv=TAB_VOISINNAGE[nc][i];
			DEGRETMP[ncv]--;	// reduction du degre des voisins du noeud courant
			/*** cas ou leurs nouveau degre est égale à 1 ***/
			if(DEGRETMP[ncv]==2){
				rc = add(ens2, &ncv);
				if(rc < 0){ perror("add failed\n"); return 1; }
			}
		}

	}

	destroy_ensemble(ens1);
	destroy_ensemble(ens2);
	free(DEGRETMP);
	return 0;
}

/*** periph(int deg) enregistre la taille de la peripherie deg 
 * de la plus grande composante connexe dans la variable static SIZE_PH ***/
/*** retourne 0 ***/
/*** retourne 1 en cas d'erreur ***/
int periph(int deg){
	int i, rc, k;
	int nc, ncv;		// noeud courant et voisins du noeud courant

	struct ensemble *ens;	// ensemble des noeuds de degre k de la plus grande composante connexe

	/*** copie du tableau de degre dans DEGRETMP ***/
	DEGRETMP = calloc(NBR_NOEUDS,sizeof(int));
	if(DEGRETMP==NULL){ perror("calloc failed\n"); return 1; }
	for(i=0; i<NBR_NOEUDS; i++){
		DEGRETMP[i]=DEGRE[i];
	}

	SIZE_PH=0;		// initialisation de la taille de la peripherie
	
	for(k=1;k<=deg;k++){
		/*** allocation de l'ensemble de degre k ***/
		ens=(struct ensemble *)new_ensemble(NBR_NOEUDS);
		if(ens == NULL){ perror("new_ensemble failed\n"); return 1; }
		/*** initialisation de l'ensemble de degre k ***/
		for(i=0; i<NBR_NOEUDS; i++){
			if(RACINES[i]==RACINE_CC && DEGRETMP[i]==k){
				rc = add(ens, &i);
				if(rc < 0){ perror("add failed\n"); return 1; }
			}
		}
		/*** enlevement des noeuds de degre k de la plus grande composante connexe ***/
		while(!isEmpty(ens)){
			nc=get(ens);
			rc = del(ens, &nc);
			if(rc != 0){ perror("del failed\n"); return 1; }
			SIZE_PH++;	// augmentation de la taille de la peripherie
			PERIPH[nc]=1;	// enregistrement du noeud courant dans le tableau de peripherie
			for(i=0; i<DEGRE[nc]; i++){
				ncv=TAB_VOISINNAGE[nc][i];
				DEGRETMP[ncv]--;	// reduction du degre des voisins du noeud courant
				/*** cas ou leurs nouveau degre est égale à k ***/
				if(DEGRETMP[ncv]==k){
					rc = add(ens, &ncv);
					if(rc < 0){ perror("add failed\n"); return 1; }
				}
			}
		}
		destroy_ensemble(ens);
	}

	free(DEGRETMP);
	return 0;
}
