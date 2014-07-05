/*
 *	Victor Oudin
 *	12/02/2014
 *
 *	utils.h
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** fonction de lecture du fichier de degre, d'allocation et d'initialisation du tableau de degre ***/
int read_degre(char * degre_file, int ** tab_degre, int nbr_noeuds);
/*** fonction de lecture du fichier .data, d'allocation et d'initialisation du tableau d'ajacence ***/
int tab_voisinnage(char * data_file, int *** tab_voisinnage, int ** tab_degre, int nbr_noeuds);
/*** fonction de lecture du fichier .data, d'allocation et d'initialisation de la matrice d'ajacence 
 * retourne le nombre de liens, ou -1 en cas d'erreur ***/
int mat_voisinnage(char * data_file, int *** mat_voisinnage, int nbr_noeuds);
/* fonction de comparaison décroissante pour qsort */
int comp(const void *a, const void *b);

int find(int ** set, int noeud);
void union_cc(int ** rang, int ** set, int noeud1, int noeud2); /*** union ***/
void link(int ** rang, int ** set, int noeud1, int noeud2);

/** cc-uf **/
/*** cc() compte le nombre de composante connexe  et le retourne ***/
/*** retourne -1 en cas d'erreur ***/
int cc();

/** ph **/
/*** premierecc() retrourne la taille de la plus grande composante connexe ***/
/*** retourne -1 en cas d'erreur ***/
int premierecc();
/*** periph2() enregistre la taille de la peripherie 2 
 * de la plus grande composante connexe dans la variable static SIZE_PH ***/
/*** retourne 0 , 1 en cas d'erreur ***/
int periph2();
/*** periph(int deg) enregistre la taille de la peripherie deg 
 * de la plus grande composante connexe dans la variable static SIZE_PH ***/
/*** retourne 0 , 1 en cas d'erreur ***/
int periph(int deg);

#endif // UTILS_H
