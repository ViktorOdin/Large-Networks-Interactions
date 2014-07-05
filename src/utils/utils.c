/*
 *	Victor Oudin
 *	12/02/2014
 *
 *	utils.c
 */
#include "utils.h"

int read_degre(char * degre_file, int ** tab_degre, int nbr_noeuds){

	FILE* fd = NULL;
	char buff[100];
	int i=0, d;
	
	(*tab_degre)= calloc(nbr_noeuds,sizeof(int));
	if((*tab_degre)==NULL){
		perror("calloc failed\n");
		return 1;
	}
	
	fd=fopen(degre_file, "rb");
	if(fd == NULL){
		perror("\n fopen() failed! \n");
		return 1;
	}

	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d\n",&d);
		(*tab_degre)[i]=d;
		i++;
	}
	fclose(fd);
	return 0;
}

int tab_voisinnage(char * data_file, int *** tab_voisinnage, int ** tab_degre, int nbr_noeuds){

	int i, a, b;
	FILE * fd = NULL;
	char buff[100];

	int *tmp = calloc(nbr_noeuds,sizeof(int));
	if(tmp==NULL){
		perror("calloc failed\n");
		return 1;
	}

	(*tab_voisinnage) = calloc(nbr_noeuds,sizeof(int *));
	if((*tab_voisinnage)==NULL){
		perror("calloc failed\n");
		return 1;
	}
	for(i=0; i<nbr_noeuds; i++){
		(*tab_voisinnage)[i] = calloc((*tab_degre)[i],sizeof(int));
		if((*tab_voisinnage)[i]==NULL){
			perror("calloc failed\n");
			return 1;
		}
	}

	fd=fopen(data_file, "rb");
	if(fd == NULL){
		perror("\n fopen() failed! \n");
		return 1;
	}

	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&a,&b);
		(*tab_voisinnage)[a][tmp[a]]=b;
		(*tab_voisinnage)[b][tmp[b]]=a;
		tmp[a]++;tmp[b]++;
	}

	fclose(fd);
	free(tmp);
	return 0;
}

int mat_voisinnage(char * data_file, int *** mat_voisinnage, int nbr_noeuds){

	int i, a, b;
	FILE * fd = NULL;
	char buff[100];

	(*mat_voisinnage) = calloc(nbr_noeuds,sizeof(int *));
	if((*mat_voisinnage)==NULL){
		perror("calloc failed\n");
		return -1;
	}
	for(i=0; i<nbr_noeuds; i++){
		(*mat_voisinnage)[i] = calloc(nbr_noeuds,sizeof(int *));
		if((*mat_voisinnage)[i]==NULL){
			perror("calloc failed\n");
			return -1;
		}
	}

	fd=fopen(data_file, "rb");
	if(fd == NULL){
		perror("\n fopen() failed! \n");
		return -1;
	}
	i=0;
	while(fgets (buff, 100, fd)!=NULL){
		sscanf(buff,"%d %d\n",&a,&b);
		if(a!=b){
			(*mat_voisinnage)[a][b]=1;
			(*mat_voisinnage)[b][a]=1;
			i++;
		}
	}

	fclose(fd);
	return i;
}

int comp(const void *a, const void *b){
	return *(int *)b - *(int *)a;
}

int find(int ** set, int noeud){
	int racine=noeud;
	while((*set)[racine]!=racine){
		racine=(*set)[racine];
	}
	int courant=noeud, tmp;
	while((*set)[courant]!=courant){
		tmp=(*set)[courant];
		(*set)[courant]=racine;
		courant=tmp;
	}
	return racine;
}

void union_cc(int ** rang, int ** set, int noeud1, int noeud2){ //union
	link(rang, set, find(set, noeud1), find(set, noeud2));
}

void link(int ** rang, int ** set, int noeud1, int noeud2){
	if((*rang)[noeud1]>(*rang)[noeud2])
		(*set)[noeud2]=noeud1;
	else{
		(*set)[noeud1]=noeud2;
		if((*rang)[noeud1]==(*rang)[noeud2] && noeud1!=noeud2)
			(*rang)[noeud2]++;
	}
}
