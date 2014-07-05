/*
 *	Victor Oudin
 *	19/02/2014
 *
 *	communaute.c
 */
#include "utils.h"

static char * DATA_FILE;			/*** fichier .data ***/
static char * DEGRE_FILE;			/*** fichier .deg ***/
static int NBR_NOEUDS;				/*** nombre de noeuds du graphe ***/
static int NBR_LIENS;				/*** nombre de liens du graphe ***/
static int * DEGRE;				/*** tableau des degre de chaque noeud ***/
static int * COMM;				/*** tableau des racines des communautés des noeuds ***/
static int * SIZE_COMM;				/*** tableau des tailles des communautés (en noeuds) ***/
static int * SOM_ARETES_COMM;			/*** tableau des sommes des arêtes ayant au moins un bout dans les communautés ***/
static int ** VOISINNAGE;			/*** tableau d'adjacence du graphe ***/
static int ** ARETES_ENTRE_COMM;		/*** matrice des arêtes entre les communautés (matrice d'adjacence au départ) ***/

inline int max(int a, int b){
	return a > b ? a : b;
}

double modular();		/*** calcul de la modularité du graphe en fonction des communautés ***/
int comm(int n1, int n2);	/*** retourne 1 si n1 et n2 font partie de la même communauté, sinon 0 ***/
double voisin(int n1, int n2);	/*** retourne 1 si n2 est voisin de n1, sinon 0 ***/

double deltaQ(int n1, int n2);	/*** calcul du delta Q après fusion des communautés n1 et n2 ***/
int optGloutQ();		/*** optimisation gloutonne de Q ***/
void MAJ(int c1, int c2);	/*** met à jour la matrice des arêtes entre communautés après fusion des communautés n1 et n2 ***/
void fusion(int c1, int c2);	/*** met à jour le tableau des racines des communautés pour la fusion des communautés n1 et n2 ***/

int main(int argc, char * argv[]){
	int i, rc, nbr_comm=0;
	double modu;
	/*** initialisation du main ***/
	if(argc != 4){
		printf("Entrez \"./communaute <graph> <size of graph> <deg file of graph>\"\n");
		return 1;
	}

	DATA_FILE = argv[1];
	NBR_NOEUDS = atoi(argv[2]);
	DEGRE_FILE= argv[3];
	
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

	/*** fabrication de la matrice d'adjacence ***/
	NBR_LIENS = mat_voisinnage(DATA_FILE, &ARETES_ENTRE_COMM, NBR_NOEUDS);
	if(NBR_LIENS < 0){
		perror("mat_voisinnage failed");
		return 1;
	}

	rc = tab_voisinnage(DATA_FILE, &VOISINNAGE, &DEGRE, NBR_NOEUDS);
	if(rc != 0){
		perror("tab_voisinnage failed");
		return 1;
	}

	/*** opttimisation gloutonne de Q  ***/
	rc = optGloutQ();
	if(rc!=0){
		perror("voisinnage failed");
		return 1;
	}

	/*** calcul de la taille des communautés et leurs nombre ***/
	SIZE_COMM = calloc(NBR_NOEUDS,sizeof(int *));
	if(SIZE_COMM==NULL){
		perror("calloc failed\n");
		return 1;
	}

	for(i=0; i<NBR_NOEUDS; i++){
		SIZE_COMM[COMM[i]]++;
	}
	for(i=0; i<NBR_NOEUDS; i++){
		if(SIZE_COMM[i]>0)
			nbr_comm++;
	}

	printf("nombre de communauté : %d\n", nbr_comm);
	modu = modular();
	printf("modularité du graphe : %f\n", modu);
	qsort(SIZE_COMM, NBR_NOEUDS, sizeof(int), comp);
	printf("nombre de noeuds dans les plus grandes communautés :\n");
	for(i=0;(i<10 && i<NBR_NOEUDS); i++)
		printf("%d\t", SIZE_COMM[i]);
	printf("\n");

	for(i=0; i<NBR_NOEUDS; i++){
		free(VOISINNAGE[i]);
	}
	free(VOISINNAGE);
	
	for(i=0; i<NBR_NOEUDS; i++){
		free(ARETES_ENTRE_COMM[i]);
	}
	free(ARETES_ENTRE_COMM);
	free(SIZE_COMM);
	free(DEGRE);
	free(COMM);
	free(SOM_ARETES_COMM);

	return 0;
}

/*** calcul de la modularité du graphe en fonction des communautés 
 ** retourne le résultat de Q=1/2m SUM ([Ai,j − ((di*dj)/2m)] ∗ δ(ci , cj )
 ***/
double modular(){
	int i,j;
	double som=0;
	for(i=0; i<NBR_NOEUDS; i++){
		for(j=0; j<NBR_NOEUDS; j++){
			if(i!=j) som += (voisin(i,j) - ((double)(DEGRE[i]*DEGRE[j])/(double)(2*NBR_LIENS)))*(double)comm(i,j);
		}
	}
	som = som / (double)(2 * NBR_LIENS);
	return som;
}

/*** retourne 1 si n1 et n2 font partie de la même communauté, sinon 0 ***/
int comm(int n1, int n2){
	if(COMM[n1]==COMM[n2])
		return 1;
	return 0;
}

/*** retourne 1 si n2 est voisin de n1, sinon 0 ***/
double voisin(int n1, int n2){
	int i;
	for(i=0;i<DEGRE[n1];i++){
		if(VOISINNAGE[n1][i]==n2)
			return 1;
	}
	return 0;
}

/*** calcul du delta Q après fusion des communautés n1 et n2
 * à l'aide de la matrice des arêtes entre les communautés
 * et du tableau de la somme des arêtes des communautés
 * ∆Q = 2(eij − ai*aj ) ***/
double deltaQ(int n1, int n2){
	double e1_2 = ((double)ARETES_ENTRE_COMM[n1][n2]/(double)(2 * NBR_LIENS));
	double a1 = ((double)SOM_ARETES_COMM[n1]/(double)(2 * NBR_LIENS));
	double a2 = ((double)SOM_ARETES_COMM[n2]/(double)(2 * NBR_LIENS));
	return (2 * (e1_2 - (a1 * a2)));
}

/*** optimisation gloutonne de Q ***/
int optGloutQ(){

	int i, j, rc, u, v, umax, vmax;
	double deltaMax=1.0, deltaCur;
	FILE * fd = NULL;
	char buff[100];

	/* initialisation du tableau des racines des communautés */
	COMM = calloc(NBR_NOEUDS,sizeof(int *));
	if(COMM==NULL){
		perror("calloc failed\n");
		return 1;
	}
	for(i=0; i<NBR_NOEUDS; i++){
		COMM[i]=i;
	}

	/* initialisation du tableau de la somme des arêtes des communautés */
	SOM_ARETES_COMM = calloc(NBR_NOEUDS,sizeof(int *));
	if(SOM_ARETES_COMM==NULL){
		perror("calloc failed\n");
		return 1;
	}
	for(i=0; i<NBR_NOEUDS; i++){
		for(j=0; j<NBR_NOEUDS; j++){
			SOM_ARETES_COMM[i]+=ARETES_ENTRE_COMM[i][j];
		}
	}

	/* ouverture du fichier .data */
	fd=fopen(DATA_FILE, "rb");
	if(fd == NULL){
		perror("fopen failed\n");
		return 1;
	}

	/* fusion de communautés tant que le delta Q augmente */
	while(deltaMax>0){
		deltaMax=0.0;
		rc=fseek(fd, 0, SEEK_SET);
		if(rc!=0){
			perror("fseek failed\n");
			return 1;
		}
		/* parcours de tous les liens du graphe par le fichier .data */
		while(fgets (buff, 100, fd)!=NULL){
			sscanf(buff,"%d %d\n",&u,&v);
			if(!comm(u,v)){
				deltaCur = deltaQ(u,v);
				/* enregistrement du delta Q max */
				if(deltaCur>deltaMax){
					deltaMax=deltaCur;
					umax=u;
					vmax=v;
				}
			}
		}
		/* fusion des communautés umax et vmax */
		MAJ(COMM[umax],COMM[vmax]);
		fusion(COMM[umax],COMM[vmax]);
	}

	fclose(fd);
	return 0;
}

/*** met à jour la matrice des arêtes entre communautés après fusion des communautés c1 et c2 ***/
void MAJ(int c1, int c2){
	int i, tmp;

	/* remise à zéro de la somme des arêtes des communautés c1 et c2 */
	SOM_ARETES_COMM[c1] = 0;
	SOM_ARETES_COMM[c2] = 0;
	/* enregistrement du nombre d'arêtes intracommunautaires de la nouvelle communauté c1 fusion c2 */
	tmp = (ARETES_ENTRE_COMM[c1][c1] + ARETES_ENTRE_COMM[c2][c1] + ARETES_ENTRE_COMM[c2][c2]);
	ARETES_ENTRE_COMM[c1][c1] = tmp;
	ARETES_ENTRE_COMM[c2][c2] = tmp;
	ARETES_ENTRE_COMM[c2][c1] = tmp;
	ARETES_ENTRE_COMM[c1][c2] = tmp;
	/* mise à jour des lignes et colonnes c1 et c2 */
	for(i=0; i<NBR_NOEUDS; i++){
		if(i!=c1 && i!=c2){
			/* enregistrement du nombre d'arêtes de la nouvelle communauté c1 fusion c2 vers la communauté i */
			tmp = (ARETES_ENTRE_COMM[c1][i] + ARETES_ENTRE_COMM[c2][i]);
			ARETES_ENTRE_COMM[c1][i] = tmp;
			ARETES_ENTRE_COMM[c2][i] = tmp;
			ARETES_ENTRE_COMM[i][c1] = tmp;		
			ARETES_ENTRE_COMM[i][c2] = tmp;
		}
		/* mise à jour de la somme des arêtes de la communauté c1 fusion c2 */
		SOM_ARETES_COMM[c1] += ARETES_ENTRE_COMM[c1][i];
		SOM_ARETES_COMM[c2] += ARETES_ENTRE_COMM[c2][i];
	}
}

/*** met à jour le tableau des racines des communautés après fusion des communautés c1 et c2 ***/
void fusion(int c1, int c2){
	int i;
	/* nouvelle racine de la communauté c2 devient c1 */
	for(i=0; i<NBR_NOEUDS; i++){
		if(COMM[i]==c2)
			COMM[i]=c1;
	}
}
