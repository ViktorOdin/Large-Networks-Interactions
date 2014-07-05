/*
 *	Victor Oudin
 *	12/02/2014
 *
 *	ensemble.h
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct ensemble ensemble;
struct ensemble * new_ensemble(unsigned int size);
void destroy_ensemble(struct ensemble * e);

int del(struct ensemble * e, const int * noeud);
int add(struct ensemble * e, const int * noeud);

int isEmpty(struct ensemble * e);
int get(struct ensemble * e);

