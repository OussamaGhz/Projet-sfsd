#include "tov.h"

//initialisation du fichierTOV
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax) {

    //verifie si le pointeur fichier est NULL
    //Raison: je previent l'acces a un pointeur non initialisé
    if (fichier == NULL) {
        printf("initialiserFichierTOV: fichier is NULL\n");
        return;
    }
    
    //initialisation des champs de l'entete
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.capaciteMax = capaciteMax;

    //alloue de la mémoire pour stocker les enregistrements, based on la capaciteMax
    fichier->enregistrements = malloc(capaciteMax * sizeof(EnregistrementPhysique));

    //vérifie si l'allocation de mémoire a echoué
    if (fichier->enregistrements == NULL) {
        printf("initialiserFichierTOV: Memory allocation failed\n");
        return;
    }

    //affichage de reussite de initialisation et initialisation de nextID à 0
    printf("initialiserFichierTOV: initialization successful\n");
    fichier->entete.nextID = 0;
}


//liberation du fichierTOV soon


//la fonction retourne un TRUE si l'ajout est reussi et false sinon , simple concept
bool ajouterEnregistrement(FichierTOV *fichier, EnregistrementPhysique *enregistrement) {

    //en ajoutant cette linge de code , le probleme de id is fixed :)
    enregistrement->entete.id = fichier->entete.nbEnregistrements;

    const char *nomFichier = "monFichierTOV.tov";

    //verification si les 2 pointeurs sont NULL pour eviter des erreurs de segmentation
    if (fichier == NULL || enregistrement == NULL) return false;

    FILE *fichierPhysique = fopen(nomFichier, "a"); // opening el file in mode append
    if (fichierPhysique == NULL) return false; //to make sure beli el file is opened

    // check la capacite maximale avant d'inserer
    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax) {
        fclose(fichierPhysique);
        return false;
    }

    enregistrement->entete.id = fichier->entete.nextID++; //new logique de id

    //j'ai utilise fprintf pour écrire les données de enregistrement dans le fichier , le format est %d | %s
    fprintf(fichierPhysique, "%d|%s\n", enregistrement->entete.id, enregistrement->data1);

    //j'augmente le compteur nbEnregistrements quand j'ajoute l'enregistrement dans le tableau "enregistrements" de "fichier"
    fichier->enregistrements[fichier->entete.nbEnregistrements] = *enregistrement;
    fichier->entete.nbEnregistrements++;

    //ferméture du fichier
    fclose(fichierPhysique);
    return true;
}


//1er test fonction suppression /soon


//utiliser la fonction dans main
//we cant use le programme yet car j'ai pas utiliser l'initialisation du fichier dans main /soon
int main() {
    FichierTOV fichier;
    BufferTransmission buffer;
    int choix, id, capaciteFichier;
    char data[TAILLE_BUFFER];
    EnregistrementPhysique newEnregistrement;

    printf("Entrez les données de l'enregistrement:\n");
    printf("data1: ");
    fgets(newEnregistrement.data1, TAILLE_MAX_ENREGISTREMENT, stdin);
    newEnregistrement.data1[strcspn(newEnregistrement.data1, "\n")] = '\0';

    if (ajouterEnregistrement(&fichier, &newEnregistrement)) {
        printf("Enregistrement ajouté avec succès.\n");
    } else {
        printf("Erreur lors de l'ajout de l'enregistrement.\n");
    }
}
