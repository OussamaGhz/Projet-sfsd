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


// first on definie la structure de l'entete de l'enregistrement phsique
typedef struct {
    int id;
    int tailleDonnees;
    int nbrEnregistrementLogique;
    int nbrChampsLogique;
} EnteteEnregistrement;

//ensuite la structure du block

typedef struct {
    EnteteEnregistrement entete;
   char dataBlock[TAILLE_MAX_Block];
} EnregistrementPhysique;

// on definie la structure de l'entete du fichier
typedef struct {
    int nbEnregistrements;
    int capaciteMax;
} EnteteFichierTOV;

// Structure du buffer
typedef struct {
    char data[TAILLE_BUFFER];
    int taille;
} BufferTransmission;

// Structure du fichier
typedef struct {
    EnteteFichierTOV entete;
    EnregistrementPhysique[MAX_ENREGISTREMENTS] enregistrements;
} FichierTOV;



#endif
