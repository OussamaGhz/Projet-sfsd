#ifndef TOV_H
#define TOV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TAILLE_MAX_ENREGISTREMENT 256
#define MAX_ENREGISTREMENTS 100
#define TAILLE_BUFFER 512

// Structure te3 l'entête enregistrement physique
typedef struct {
    int id;             
    int tailleDonnees;  
} EnteteEnregistrement;


typedef struct {
    EnteteEnregistrement entete;
    char donnees[TAILLE_MAX_ENREGISTREMENT];  
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
