#ifndef TOV_H
#define TOV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TAILLE_MAX_Block 256
#define MAX_ENREGISTREMENTS 100
#define TAILLE_BUFFER 512
#define SEPARATEUR '|' 
#define SIZE 30

// first on definie la structure de l'entete de l'enregistrement phsique
typedef struct {
    int id;
    int tailleDonnees;
    int nbrEnregistrementLogique;
} EnteteEnregistrement;

//ensuite la structure du block

typedef struct {
    EnteteEnregistrement entete;
   char dataBlock[TAILLE_MAX_Block];
} EnregistrementPhysique;

// on definie la structure de l'entete du fichier
typedef struct {
    int nbEnregistrementslogique;
    int nbrBlock;
    int facteurdeblockage;
    int nextId;
} EnteteFichierTOV;

// Structure du buffer
typedef struct {
    char data[TAILLE_BUFFER];
    int taille;
} BufferTransmission;

// Structure du fichier
typedef struct {
    EnteteFichierTOV entete;
    EnregistrementPhysique enregistrements[MAX_ENREGISTREMENTS];
} FichierTOV;

typedef struct{
 int id; //ajout de l'id etudiant pour la designe en tant que cle de depart 
 char nom[20];
 char *prenom[5];
 int matricule;
 char filiere[20];
 int suplogique;
 int taille;
}enregistrementLogique;

typedef struct {
  enregistrementLogique classe[MAX_ENREGISTREMENTS];
  int nbrEnregistrementLogique;
}fichierLogique;

typedef struct ElementTablHachabl {
    int id;
    int idBlock;
    struct ElementTablHachabl* next;
} ElementTablHachabl;

typedef struct HashTable {
    ElementTablHachabl* tab[SIZE];
} HashTable;



#endif
