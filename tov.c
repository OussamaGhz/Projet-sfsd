#include "tov.h"




//1st try to use file functions in ajouterEnregistrment:
bool ajouterEnregistrement(FichierTOV *fichier, EnregistrementPhysique *enregistrement) {
    const char *nomFichier = "monFichierTOV.tov";
    if (fichier == NULL || enregistrement == NULL) return false;

    FILE *fichierPhysique = fopen(nomFichier, "a"); // Mode texte bch najoutiw
    if (fichierPhysique == NULL) return false;

    // check la capacite maximale avant d'inserer
    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax) {
        fclose(fichierPhysique);
        return false;
    }

    // Écriture formatée dans le fichier
    fprintf(fichierPhysique, "%d|%s\n", enregistrement->entete.id, enregistrement->data1);

    // Ajout en mémoire
    fichier->enregistrements[fichier->entete.nbEnregistrements] = *enregistrement;
    fichier->entete.nbEnregistrements++;

    fclose(fichierPhysique);
    return true;
}





//utiliser la fonction dans main
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
