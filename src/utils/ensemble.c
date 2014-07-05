/*
 *	Victor Oudin
 *	12/02/2014
 *
 *	ensemble.c
 */
#include "ensemble.h"

typedef struct ensemble {
	unsigned int nbr_elements;
	int* element;
	unsigned int taille_max;
	int* index;
} ensemble;

struct ensemble * new_ensemble(unsigned int size){
	ensemble *e=malloc(sizeof(struct ensemble));
	if(e==NULL){
		free(e);
		return NULL;
	}

	e->nbr_elements=0;
	e->taille_max=size;

	e->element= calloc(size,sizeof(int));
	if(e->element==NULL){
		free(e->element);
		free(e);
		return NULL;
  	}

	e->index= calloc(size,sizeof(int));
	if(e->index==NULL){
		free(e->element);
		free(e->index);
		free(e);
		return NULL;
  	}

	return e;
}

void destroy_ensemble(struct ensemble * e){
	free(e->element);
	free(e->index);
	return free(e);
}


int del(struct ensemble * e, const int * noeud){
	if(e==NULL){
		perror("\n wrong argument give to del\n");
		return 1;
	}

	int i=e->index[*noeud];
	e->nbr_elements--;
	e->element[i]=e->element[e->nbr_elements];
	e->index[e->element[i]]=i;
	e->index[*noeud]=-1;

	return 0;
}

int add(struct ensemble * e, const int * noeud){
	if(e==NULL){
		perror("\n wrong argument give to add\n");
		return 1;
	}

	e->element[e->nbr_elements]=*noeud;
	e->index[*noeud]=e->nbr_elements;
	e->nbr_elements++;

	return 0;
}

int isEmpty(struct ensemble * e){
	return (e->nbr_elements==0);
}

int get(struct ensemble * e){
	return e->element[0];
}
