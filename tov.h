#ifndef TOV_H
#define TOV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TAILLE_MAX_ENREGISTREMENT 256
#define MAX_ENREGISTREMENTS 100
#define TAILLE_BUFFER 512
#define SEPARATEUR '|'



/*sert a fournir des informations de base sur chaque enregistrement comme son id qui est important
pour le suivi des enregistrements dans le fichier et la table de hachage*/

typedef struct {
    int id; //ce champ est utilise pour identifier de maniere unique chaque enregistrement dans le fichier TOV (its used in most functions)
    int tailleBlocR; //taille block reel ( not used in code , it had a role apr the idea was canceled)
} EnteteEnregistrement;


//adding new champs à l'enregistrement physique
typedef struct {
    EnteteEnregistrement entete;
    //ces trois champs representent les donnees stockees dans chaque enregistrement:
    char data1[TAILLE_MAX_ENREGISTREMENT];
    char data2[TAILLE_MAX_ENREGISTREMENT];
    char data3[TAILLE_MAX_ENREGISTREMENT];
    //chaque data peut stocker une chaine de caracteres de n'importe quelle taille mais < TAILLE_MAX_ENREGISTREMENT
    //donc la taille est variable
} EnregistrementPhysique;


//cette structure est utilisee pour implementer une table de hachage
typedef struct {
    /*les indices are calculated by hash function and id of enregistrements are stocked here*/
    int *table;
    //ce champ (taille) stocke la taille de la table de hachage, cad le nombre d'elements qu'elle peut contenir
    int taille;t
} HashTable;


//cette structure represente les informations globales concernant le fichier TOV
typedef struct {
    int nbEnregistrements; //ce champ stocke le nombre actuel d'enregistrements dans le fichier TOV
    int capaciteMax;       //le nombre maximum d'enregistrements que le fichier peut contenir
    int nextID;            // nouveau champ pour suivre l'id global
} EnteteFichierTOV;



//Structure te3 buffer de transmission
typedef struct {
    //c un tableau de caracteres used pour stocker temporairement des donnees a transmettre
    char data[TAILLE_BUFFER];
    // Ce champ is used pour suivre la quantite de donnees actuellement stockees dans le tampon
    int taille;
} BufferTransmission;



//cette structure represente le fichier TOV
typedef struct {
    EnteteFichierTOV entete;
    //c un pointeur vers un tableau d'enregistrements physiques , ce tableau stocke les enregistrements individuels contenus dans le fichier TOV
    EnregistrementPhysique *enregistrements;
} FichierTOV;



// Prototypes de fonctions pour la gestion du fichier TOV

//Verifiez si fichier n'est pas NULL
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax);

//Verifiez si fichier n'est pas NULL
void libererFichierTOV(FichierTOV *fichier);

//Verifiez si fichier et le Buffer ne sont pas NULL
bool ajouterEnregistrement(FichierTOV *fichier, HashTable *hashTable, EnregistrementPhysique *enregistrement);

//Verifiez si fichier n'est pas NULL
bool supprimerEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id);

//Verifiez si fichier n'est pas NULL
EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id); //changed to fit the new function

//Verifiez si fichier n'est pas NULL
void afficherFichierTOV(const FichierTOV *fichier);


// pour hed les 2 procedures verifiez les tailles bch n'evitiw le depassement de buffer (hws 3liha)
void remplirBuffer(BufferTransmission *buffer, const char *data);
void viderBuffer(BufferTransmission *buffer);


/*cette fonction calcule la taille de chaque enregistrement , heka n9dro ndiro le test w nchofo beli sah
la taille des enregistrements est variable w ttbdl d'apres wch ndkhlo hna*/
unsigned long CalculerTailleEnregistrement(const EnregistrementPhysique *enregistrement);


#endif
