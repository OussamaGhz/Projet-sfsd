#include "tov.h"

/*                                                            *  DONE  BY JINX  *                                                        */

// nouvelle fonction d'initialisation de la table hashage:          (checked)
void initialiserHashTable(HashTable *hashTable, int taille)
{
    // j'alloue de la memoire pour le tableau de la table de hachage
    hashTable->table = malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++)
    {
        hashTable->table[i] = -1; // on les mets tous à -1 pour les declarer vide
    }
    hashTable->taille = taille; // la taille de la table de hachage
}

// it's role est de transformer un ID en un indice
int hashFunction(int id, int tailleTable)
{
    // simple hach use of modulo
    // this will assure que l'indice is always dans les limites de la table de hachage (de 0 a tailleTable - 1).
    return id % tailleTable;
}

// initialisation du fichierTOV                           (checked)
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

// remaking the function libierFichierTOV and fixing it's problems         (checked)
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

// la fonction d'insertion                  (checked)
bool ajouterEnregistrement(FichierTOV *fichier, HashTable *hashTable, EnregistrementPhysique *enregistrement, BufferTransmission *buffer)
{
    if (fichier == NULL || enregistrement == NULL || buffer == NULL)
        return false;

    const char *nomFichier = "monFichierTOV.tov";
    FILE *fichierPhysique = fopen(nomFichier, "a");
    if (fichierPhysique == NULL)
        return false;

    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax)
    {
        fclose(fichierPhysique);
        return false;
    }

    enregistrement->entete.id = fichier->entete.nextID++;

    // Utilize buffer for data1
    remplirBuffer(buffer, enregistrement->data1);
    fprintf(fichierPhysique, "%d|%s|%s|%s\n",
            enregistrement->entete.id,
            buffer->data, // Using buffer's data
            enregistrement->data2,
            enregistrement->data3);
    viderBuffer(buffer);

    fichier->enregistrements[fichier->entete.nbEnregistrements] = *enregistrement;
    fichier->entete.nbEnregistrements++;

    fclose(fichierPhysique);

    int indexHash = hashFunction(enregistrement->entete.id, hashTable->taille);
    hashTable->table[indexHash] = enregistrement->entete.id;

    return true;
}

// supprimer un enregistrement specified with his ID du fichier TOV:     (checked)
bool supprimerEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id, BufferTransmission *buffer)
{
    const char *nomFichier = "monFichierTOV.tov";
    const char *nomFichierTemp = "tempFichierTOV.tov";

    if (fichier == NULL || buffer == NULL)
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
            remplirBuffer(buffer, temp.data1); // adding the data to the buffer
        }
    }

    fclose(fichierPhysique);
    fclose(fichierTemp);

    if (found)
    {
        remove(nomFichier);
        rename(nomFichierTemp, nomFichier);

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

    int indexHash = hashFunction(id, hashTable->taille);
    hashTable->table[indexHash] = -1;

    viderBuffer(buffer); // Clearing the buffer after use

    return found;
}

// fonction de recherche optimised to perfection using hachage:        (checked)
EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id)
{
    if (fichier == NULL || hashTable == NULL)
    {
        return NULL;
    }
    /*calcule l'indice dans la table de hachage pour l'id donne en utilisant la fonction de hachage*/
    int index = hashFunction(id, hashTable->taille);

    // verifying if id is stocked at the this index
    if (hashTable->table[index] == id)
    {
        // find l'enregistrement dans le tableau "enregistrements" de "fichier"
        for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
        {
            if (fichier->enregistrements[i].entete.id == id)
            {
                return &fichier->enregistrements[i];
            }
        }
    }
    return NULL; // enregistrement not found
}

// fonction pour afficher le contenue de fichier: (pas importante while using GTK)     (checked)
void afficherFichierTOV(const FichierTOV *fichier)
{
    if (fichier == NULL)
        return;
    // boucle simple pour afficher les enregistrement et leur contenue
    printf("Fichier TOV contient %d enregistrements:\n", fichier->entete.nbEnregistrements);
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        printf("Enregistrement %d:\n", i);
        printf("data1: %s\n", fichier->enregistrements[i].data1);
        printf("data2: %s\n", fichier->enregistrements[i].data2);
        printf("data3: %s\n", fichier->enregistrements[i].data3);
    }
}

// utiliser le buffer pour gerer le tampon de transmission avec ces 2 fonctions   (checked)
void remplirBuffer(BufferTransmission *buffer, const char *data)
{
    if (buffer == NULL || data == NULL)
        return;
    // copy data to the buffer, leaving space for the null terminator
    strncpy(buffer->data, data, TAILLE_BUFFER - 1);
    // explicitly set the last character to '\0' for null termination
    buffer->data[TAILLE_BUFFER - 1] = '\0';
    // Update the size of the data in the buffer
    buffer->taille = strlen(buffer->data);
}

void viderBuffer(BufferTransmission *buffer)
{ //(checked)
    if (buffer == NULL)
        return;
    // set the first character of the buffer to '\0' pour indiquer an empty string
    buffer->data[0] = '\0';
    // reset the size of the data in the buffer to 0
    buffer->taille = 0;
}

// une fonction pas demandee , j vais l'utiliser juste pour calculer la taille des diff enregistrements
unsigned long CalculerTailleEnregistrement(const EnregistrementPhysique *enregistrement)
{
    if (enregistrement == NULL)
        return 0;

    return sizeof(EnteteEnregistrement) + strlen(enregistrement->data1);
}

// utiliser les fonctions dans main
#include "tov.h"
