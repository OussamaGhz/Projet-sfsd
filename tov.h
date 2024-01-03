#ifndef TOV_H
#define TOV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*#define TAILLE_MAX_ENREGISTREMENT 256 | useless since la taille welat doka variable */
#define MAX_ENREGISTREMENTS 100
#define TAILLE_BUFFER 512

/**
 * Structure te3 l'entête enregistrement physique
 * Contient l'ID de l'enregistrement et la taille des donnees.
 */
typedef struct
{
    int id;
    int tailleDonnees;
} EnteteEnregistrement;

// 1er test de separer les enregistrement avec des caracteres:
typedef struct
{
    EnteteEnregistrement entete;
    char *donnees; // Pointeur vers un tableau de caractères
    char separateur;
} EnregistrementPhysique;

typedef struct
{
    int nbEnregistrements;
    int capaciteMax;
} EnteteFichierTOV;

// Structure te3 buffer de transmission
typedef struct
{
    char data[TAILLE_BUFFER];
    int taille;
} BufferTransmission;

// Structure pour le fichier TOV sans chevauchement
typedef struct
{
    EnteteFichierTOV entete;
    EnregistrementPhysique *enregistrements;
} FichierTOV;

// Prototypes de fonctions pour la gestion du fichier TOV

// Verifiez si fichier n'est pas NULL
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax);

// Verifiez si fichier n'est pas NULL
void libererFichierTOV(FichierTOV *fichier);

// Verifiez si fichier et le Buffer ne sont pas NULL
bool ajouterEnregistrement(FichierTOV *fichier, BufferTransmission *buffer);

// Verifiez si fichier n'est pas NULL
bool supprimerEnregistrement(FichierTOV *fichier, int id);

// Verifiez si fichier n'est pas NULL
EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, int id);

// Verifiez si fichier n'est pas NULL
void afficherFichierTOV(const FichierTOV *fichier);

// pour hed les 2 procedures verifiez les tailles bch n'evitiw le depassement de buffer (hws 3liha)
void remplirBuffer(BufferTransmission *buffer, const char *data);
void viderBuffer(BufferTransmission *buffer);

/*cette fonction calcule la taille de chaque enregistrement , heka n9dro ndiro le test w nchofo beli sah
la taille des enregistrements est variable w ttbdl d'apres wch ndkhlo hna*/
unsigned long CalculerTailleEnregistrement(const EnregistrementPhysique *enregistrement);

#endif
