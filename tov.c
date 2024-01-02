#include <stdio.h>

#include "tov.h"

void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax)
{
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.capaciteMax = capaciteMax;
    fichier->enregistrements = (EnregistrementPhysique *)malloc(capaciteMax * sizeof(EnregistrementPhysique));

    if (fichier->enregistrements == NULL)
    {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < capaciteMax; i++)
    {
        fichier->enregistrements[i].donnees = NULL; // Initialiser les pointeurs à NULL
    }
}

void libererFichierTOV(FichierTOV *fichier)
{
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        free(fichier->enregistrements[i].donnees);
    }
    free(fichier->enregistrements);
}

bool ajouterEnregistrement(FichierTOV *fichier, BufferTransmission *buffer)
{
    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax)
    {
        return false;
    }

    EnregistrementPhysique *enreg = &fichier->enregistrements[fichier->entete.nbEnregistrements];

    enreg->donnees = (char *)malloc(buffer->taille * sizeof(char));
    if (enreg->donnees == NULL)
    {
        return false;
    }

    strncpy(enreg->donnees, buffer->data, buffer->taille);
    enreg->entete.id = fichier->entete.nbEnregistrements;
    enreg->entete.tailleDonnees = buffer->taille;
    enreg->separateur = '|'; // Séparateur choisi

    fichier->entete.nbEnregistrements++;
    return true;
}

bool supprimerEnregistrement(FichierTOV *fichier, int id)
{
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        if (fichier->enregistrements[i].entete.id == id)
        {
            free(fichier->enregistrements[i].donnees);

            for (int j = i; j < fichier->entete.nbEnregistrements - 1; j++)
            {
                fichier->enregistrements[j] = fichier->enregistrements[j + 1];
            }

            fichier->entete.nbEnregistrements--;
            return true;
        }
    }
    return false;
}

int main()
{

    printf("Hello, sohaib");

    return 0;
}
