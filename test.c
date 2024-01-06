<<<<<<< HEAD
#include "tov.h"

EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, int id) { 
    for (int i = 0; i < fichier->entete.nbEnregistrements; ++i) {
        if (fichier->enregistrements[i].entete.id == id) {
            return &fichier->enregistrements[i];
        }
    }
    return NULL;
}

void afficherFichierTOV(const FichierTOV *fichier) { 
    printf("Fichier TOV contient %d enregistrements:\n", fichier->entete.nbEnregistrements);
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++) {
        printf("Enregistrement %d: %s\n", fichier->enregistrements[i].entete.id, fichier->enregistrements[i].donnees);
    }
}


void remplirBuffer(BufferTransmission *buffer, const char *data) {
    strncpy(buffer->data, data, TAILLE_BUFFER);
    buffer->taille = strlen(buffer->data);
}

void viderBuffer(BufferTransmission *buffer) { 
    memset(buffer->data, 0, TAILLE_BUFFER);
    buffer->taille = 0;
}

int main() {
    printf("Hello, Rayane!");
=======
#include <stdio.h>

int main() {
    printf("Hello, hhhh");
>>>>>>> 7dea2334924ad46f27ff21066786920d5d7d508a
    return 0;
}
