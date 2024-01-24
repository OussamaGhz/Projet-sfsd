// algorithmes
#include "tov.h"
void initialiserHashTable(HashTable *hashTable, int taille)
{
    hashTable->table = malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++)
    {
        hashTable->table[i] = -1; // if empty then -1
    }
    hashTable->taille = taille;
}

// it's role est de transformer un ID en un indice
int hashFunction(int id, int tailleTable)
{
    return id % tailleTable;
}

// initialisation du fichierTOV
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax)
{

    // verifie si le pointeur fichier est NULL
    // Raison: je previent l'acces a un pointeur non initialisé
    if (fichier == NULL)
    {
        printf("initialiserFichierTOV: fichier is NULL\n");
        return;
    }

    // initialisation des champs de l'entete
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.capaciteMax = capaciteMax;

    // alloue de la mémoire pour stocker les enregistrements, based on la capaciteMax
    fichier->enregistrements = malloc(capaciteMax * sizeof(EnregistrementPhysique));

    // vérifie si l'allocation de mémoire a echoué
    if (fichier->enregistrements == NULL)
    {
        printf("initialiserFichierTOV: Memory allocation failed\n");
        return;
    }

    // affichage de reussite de initialisation et initialisation de nextID à 0
    printf("initialiserFichierTOV: initialization successful\n");
    fichier->entete.nextID = 0;
}

bool ajouterEnregistrement(FichierTOV *fichier, HashTable *hashTable, EnregistrementPhysique *enregistrement)
{
    // verification si les 2 pointeurs sont NULL pour eviter des erreurs de segmentation
    if (fichier == NULL || enregistrement == NULL)
        return false;

    const char *nomFichier = "monFichierTOV.tov";

    FILE *fichierPhysique = fopen(nomFichier, "a");
    if (fichierPhysique == NULL)
        return false;

    // check la capacite maximale avant d'inserer
    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax)
    {
        fclose(fichierPhysique);
        return false;
    }

    // en ajoutant cette linge de code , le probleme de id is fixed :)
    enregistrement->entete.id = fichier->entete.nextID++;

    // ecrire les donnees de l'enregistrement dans le fichier
    fprintf(fichierPhysique, "%d|%s|%s|%s\n",
            enregistrement->entete.id,
            enregistrement->data1,
            enregistrement->data2,
            enregistrement->data3);

    // ajouter l'enregistrement dans le tableau en memoire
    fichier->enregistrements[fichier->entete.nbEnregistrements] = *enregistrement;
    fichier->entete.nbEnregistrements++;

    fclose(fichierPhysique);

    // mise à jour de la table de hashage
    int indexHash = hashFunction(enregistrement->entete.id, hashTable->taille);
    hashTable->table[indexHash] = enregistrement->entete.id;

    return true;
}

// remaking the function libierFichierTOV and fixing it's problems
void libererFichierTOV(FichierTOV *fichier)
{
    // verifier si le fichier est null
    if (fichier == NULL)
    {
        printf("libererFichierTOV: fichier est NULL\n");
        return;
    }

    // supprimer le fichier physique
    const char *nomFichier = "monFichierTOV.tov";
    remove(nomFichier);

    // liberer the memory of les enregistrements
    free(fichier->enregistrements);
    fichier->enregistrements = NULL;
    fichier->entete.nbEnregistrements = 0;

    printf("libererFichierTOV: memoire liberer et fichier supprime\n");
}

bool supprimerEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id)
{

    const char *nomFichier = "monFichierTOV.tov";
    // cree et open un fichier temporaire pour stocker temporairement les enregistrements qui ne sont pas supprimes.
    const char *nomFichierTemp = "tempFichierTOV.tov";

    if (fichier == NULL)
        return false;

    FILE *fichierPhysique = fopen(nomFichier, "r");
    FILE *fichierTemp = fopen(nomFichierTemp, "w");

    if (fichierPhysique == NULL || fichierTemp == NULL)
    {
        if (fichierPhysique != NULL)
            fclose(fichierPhysique);
        if (fichierTemp != NULL)
            fclose(fichierTemp);
        return false;
    }

    char ligne[1024];
    EnregistrementPhysique temp;
    bool found = false;
    int indexFound = -1;

    for (int i = 0; fgets(ligne, sizeof(ligne), fichierPhysique) != NULL; ++i)
    {
        sscanf(ligne, "%d|%s", &temp.entete.id, temp.data1);
        if (temp.entete.id != id)
        {
            fputs(ligne, fichierTemp);
        }
        else
        {
            found = true;
            indexFound = i;
        }
    }

    fclose(fichierPhysique);
    fclose(fichierTemp);

    if (found)
    {
        // remplace l'ancien fichier par le nouveau
        remove(nomFichier);
        rename(nomFichierTemp, nomFichier);

        // mettre à jour la structure en memoire
        for (int i = indexFound; i < fichier->entete.nbEnregistrements - 1; ++i)
        {
            fichier->enregistrements[i] = fichier->enregistrements[i + 1];
        }
        fichier->entete.nbEnregistrements--;

        memset(&fichier->enregistrements[fichier->entete.nbEnregistrements], 0, sizeof(EnregistrementPhysique));
    }
    else
    {
        remove(nomFichierTemp);
    }

    // same thing kima wch derna f la fonction ajouter enregistrement
    int indexHash = hashFunction(id, hashTable->taille);
    // bch nbyno beli cet emplacement est vide dans la table de hashage
    hashTable->table[indexHash] = -1;

    return found;
}

void afficherFichierTOV(const FichierTOV *fichier)
{
    if (fichier == NULL)
        return;
    // bouvle simple pour afficher les enregistrement et leur contenue
    printf("Fichier TOV contient %d enregistrements:\n", fichier->entete.nbEnregistrements);
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        printf("Enregistrement %d:\n", i);
        printf("data1: %s\n", fichier->enregistrements[i].data1);
    }
}