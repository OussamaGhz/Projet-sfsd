#ifndef TOV_H
#define TOV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*#define TAILLE_MAX_ENREGISTREMENT 256 | useless since la taille welat doka variable */
#define MAX_ENREGISTREMENTS 100
#define TAILLE_BUFFER 512

// Structure te3 l'entête enregistrement physique
typedef struct {
    int id;
    int tailleDonnees;
} EnteteEnregistrement;

/*code jdid te3 enregistrement physique , 2 eme champ ki rah ypointi 3la tableau de caracteres
la taille des donnees est rendue variable psq st3mlna un pointeur char *donnees
f placet d'un tableau de taille fixe*/

typedef struct {
    EnteteEnregistrement entete;
    char *donnees;  // Pointeur vers un tableau de caracteres
} EnregistrementPhysique;


typedef struct {
    int nbEnregistrements;
    int capaciteMax;
} EnteteFichierTOV;

// Structure te3 buffer de transmission
typedef struct {
    char data[TAILLE_BUFFER];
    int taille;
} BufferTransmission;

// Structure pour le fichier TOV sans chevauchement
typedef struct {
    EnteteFichierTOV entete;
    EnregistrementPhysique *enregistrements;
} FichierTOV;

// Prototypes de fonctions pour la gestion du fichier TOV
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax);
void libererFichierTOV(FichierTOV *fichier);
bool ajouterEnregistrement(FichierTOV *fichier, BufferTransmission *buffer);
bool supprimerEnregistrement(FichierTOV *fichier, int id);
EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, int id);
void afficherFichierTOV(const FichierTOV *fichier);
void remplirBuffer(BufferTransmission *buffer, const char *data);
void viderBuffer(BufferTransmission *buffer);

#endif
