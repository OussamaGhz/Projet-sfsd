#include "tov.h"

/*                                                            *  DONE  BY JINX  *                                                        */


//nouvelle fonction d'initialisation de la table hashage:          (checked)
void initialiserHashTable(HashTable *hashTable, int taille) {
    //j'alloue de la memoire pour le tableau de la table de hachage
    hashTable->table = malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++) {
        hashTable->table[i] = -1;  //on les mets tous à -1 pour les declarer vide
    }
    hashTable->taille = taille; //la taille de la table de hachage
}

//it's role est de transformer un ID en un indice
int hashFunction(int id, int tailleTable) {
    //simple hach use of modulo
    //this will assure que l'indice is always dans les limites de la table de hachage (de 0 a tailleTable - 1).
    return id % tailleTable;
}


//initialisation du fichierTOV                           (checked)
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


//remaking the function libierFichierTOV and fixing it's problems         (checked)
void libererFichierTOV(FichierTOV *fichier) {
    //verifier si le fichier est null
    if (fichier == NULL) {
        printf("libererFichierTOV: fichier est NULL\n");
        return;
    }

    //supprimer le fichier physique
    const char *nomFichier = "monFichierTOV.tov";
    remove(nomFichier);

    //liberer the memory of les enregistrements
    free(fichier->enregistrements);
    fichier->enregistrements = NULL;
    fichier->entete.nbEnregistrements = 0;

    printf("libererFichierTOV: memoire liberer et fichier supprime\n");
}




//la fonction d'insertion                  (checked)
bool ajouterEnregistrement(FichierTOV *fichier, HashTable *hashTable, EnregistrementPhysique *enregistrement) {

    //verification si les 2 pointeurs sont NULL pour eviter des erreurs de segmentation
    if (fichier == NULL || enregistrement == NULL) return false;

    const char *nomFichier = "monFichierTOV.tov";

    FILE *fichierPhysique = fopen(nomFichier, "a"); //we write à la fin du fichier sans ecraser son contenu actuel
    if (fichierPhysique == NULL) return false;

     //check la capacite maximale avant d'inserer
    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax) {
        fclose(fichierPhysique);
        return false;
    }

    //en ajoutant cette linge de code , le probleme de id is fixed :)
    enregistrement->entete.id = fichier->entete.nextID++; //chaque fois on ajoute un enregistrement on incremente

    //ecrire les donnees de l'enregistrement dans le fichier
    fprintf(fichierPhysique, "%d|%s|%s|%s\n",
            enregistrement->entete.id,
            enregistrement->data1,
            enregistrement->data2,
            enregistrement->data3);

    //ajouter l'enregistrement dans le tableau en memoire
    fichier->enregistrements[fichier->entete.nbEnregistrements] = *enregistrement;
    fichier->entete.nbEnregistrements++;

    fclose(fichierPhysique);

    //mise à jour de la table de hashage
    int indexHash = hashFunction(enregistrement->entete.id, hashTable->taille); //appelle la fonction hashFunction, en lui passant l'id de l'enregistrement
    hashTable->table[indexHash] = enregistrement->entete.id;

    return true;
}


//supprimer un enregistrement specified with his ID du fichier TOV:     (checked)
bool supprimerEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id) {

    const char *nomFichier = "monFichierTOV.tov";
    //cree et open un fichier temporaire pour stocker temporairement les enregistrements qui ne sont pas supprimes.
    const char *nomFichierTemp = "tempFichierTOV.tov";

    if (fichier == NULL) return false;

    //ouvrir le fichier d'origine en mode lecture en utilisant fopen et stocke le pointeur dans fichierPhysique
    FILE *fichierPhysique = fopen(nomFichier, "r");
    //ouvrir le fichier temporaire en mode ecriture en utilisant fopen et stocke le pointeur dans fichierTemp
    FILE *fichierTemp = fopen(nomFichierTemp, "w");


    //cela va confirmer que la suppression n'est pas effectuee si l'ouverture des fichiers de donnees has failed
    if (fichierPhysique == NULL || fichierTemp == NULL) {
        if (fichierPhysique != NULL) fclose(fichierPhysique);
        if (fichierTemp != NULL) fclose(fichierTemp);
        return false;
    }

    //variables
    char ligne[1024];
    EnregistrementPhysique temp;
    bool found = false;
    int indexFound = -1;

    //lecture des enregistrements
    for (int i = 0; fgets(ligne, sizeof(ligne), fichierPhysique) != NULL; ++i) {

        //chaque ligne est stockee dans la variable ligne
        sscanf(ligne, "%d|%s", &temp.entete.id, temp.data1);
        /*la fonction compare l'ID de chaque enregistrement a l'ID given by user.
        Si l'ID correspond, this means que l'enregistrement doit etre deleted.*/
        if (temp.entete.id != id) {
            fputs(ligne, fichierTemp);
        } else {
        //La variable found est definie à true et l'indice de l'enregistrement a supprimer (indexFound) est stocke.
            found = true;
            indexFound = i;
        }
    }

    fclose(fichierPhysique);
    fclose(fichierTemp);

    //si un enregistrement avec l'ID specifie a ete trouve ( foud == true)
    if (found) {
        //remplace l'ancien fichier par le nouveau
        remove(nomFichier);
        rename(nomFichierTemp, nomFichier);

        //mettre à jour la structure en memoire
        for (int i = indexFound; i < fichier->entete.nbEnregistrements - 1; ++i) {
            fichier->enregistrements[i] = fichier->enregistrements[i + 1];
        }
        fichier->entete.nbEnregistrements--;
        
        //reinitialise l'emplacement de l'enregistrement supprime avec des zeros en utilisant memset
        memset(&fichier->enregistrements[fichier->entete.nbEnregistrements], 0, sizeof(EnregistrementPhysique));
    } else {
        remove(nomFichierTemp);
    }

    //same thing kima wch derna f la fonction ajouter enregistrement
    int indexHash = hashFunction(id, hashTable->taille);
    //bch nbyno beli cet emplacement est vide dans la table de hashage apres la suppresion
    hashTable->table[indexHash] = -1;

    return found;
}


//fonction de recherche optimised to perfection using hachage:        (checked)
EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id) {
    if (fichier == NULL || hashTable == NULL) {
        return NULL;
    }
    /*calcule l'indice dans la table de hachage pour l'id donne en utilisant la fonction de hachage*/
    int index = hashFunction(id, hashTable->taille);
    
    //verifying if id is stocked at the this index
    if (hashTable->table[index] == id) {
        //find l'enregistrement dans le tableau "enregistrements" de "fichier"
        for (int i = 0; i < fichier->entete.nbEnregistrements; i++) {
            if (fichier->enregistrements[i].entete.id == id) {
                return &fichier->enregistrements[i];
            }
        }
    }
    return NULL; //enregistrement not found
}


//fonction pour afficher le contenue de fichier: (pas importante while using GTK)     (checked)
void afficherFichierTOV(const FichierTOV *fichier) {
    if (fichier == NULL) return;
    //boucle simple pour afficher les enregistrement et leur contenue 
    printf("Fichier TOV contient %d enregistrements:\n", fichier->entete.nbEnregistrements);
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++) {
        printf("Enregistrement %d:\n", i);
        printf("data1: %s\n", fichier->enregistrements[i].data1);
        printf("data2: %s\n", fichier->enregistrements[i].data2);
        printf("data3: %s\n", fichier->enregistrements[i].data3);
    }
}


//utiliser le buffer pour gerer le tampon de transmission avec ces 2 fonctions   (checked)
void remplirBuffer(BufferTransmission *buffer, const char *data) {
    if (buffer == NULL || data == NULL) return;
    //copy data to the buffer, leaving space for the null terminator
    strncpy(buffer->data, data, TAILLE_BUFFER - 1);
    //explicitly set the last character to '\0' for null termination
    buffer->data[TAILLE_BUFFER - 1] = '\0';
    //Update the size of the data in the buffer
    buffer->taille = strlen(buffer->data);
}



void viderBuffer(BufferTransmission *buffer) {    //(checked)
    if (buffer == NULL) return;
    //set the first character of the buffer to '\0' pour indiquer an empty string
    buffer->data[0] = '\0';
    //reset the size of the data in the buffer to 0
    buffer->taille = 0;
}



//une fonction pas demandee , j vais l'utiliser juste pour calculer la taille des diff enregistrements
unsigned long CalculerTailleEnregistrement(const EnregistrementPhysique *enregistrement) {
    if (enregistrement == NULL) return 0;

    return sizeof(EnteteEnregistrement) + strlen(enregistrement->data1);
}



//utiliser les fonctions dans main
int main() {
    //explication des variables
    FichierTOV fichier;
    HashTable hashTable;
    BufferTransmission buffer;
    int choix, id, capaciteFichier;
    char data[TAILLE_BUFFER];
    EnregistrementPhysique newEnregistrement;

    printf("Entrez la capacité maximale du fichier TOV: ");
    scanf("%d", &capaciteFichier);
    getchar();
    
    initialiserFichierTOV(&fichier, capaciteFichier);
    initialiserHashTable(&hashTable, capaciteFichier);


    while(1)
    {
        printf("\nChoisissez une operation:\n");
        printf("1- Ajouter un enregistrement\n");
        printf("2- Supprimer un enregistrement\n");
        printf("3- Rechercher un enregistrement\n");
        printf("4- Afficher le contenu du fichier\n");
        printf("5- Quitter le programme\n");
        printf("Votre choix: ");
        scanf("%d", &choix);
        getchar();
        
        
        switch(choix)
        {

            //changment dans le cas d'ajouter un enregistrement pour avoir 3 champs
            case 1: {
                printf("Entrez les donnees de l'enregistrement:\n");
                printf("data1: ");
                fgets(newEnregistrement.data1, TAILLE_MAX_ENREGISTREMENT, stdin);
                newEnregistrement.data1[strcspn(newEnregistrement.data1, "\n")] = '\0';

                printf("data2: ");
                fgets(newEnregistrement.data2, TAILLE_MAX_ENREGISTREMENT, stdin);
                newEnregistrement.data2[strcspn(newEnregistrement.data2, "\n")] = '\0';

                printf("data3: ");
                fgets(newEnregistrement.data3, TAILLE_MAX_ENREGISTREMENT, stdin);
                newEnregistrement.data3[strcspn(newEnregistrement.data3, "\n")] = '\0';

                if (ajouterEnregistrement(&fichier, &hashTable, &newEnregistrement)){
                    printf("Enregistrement ajoute avec succes\n");
                }
                else{
                    printf("Erreur lors de l'ajout de l'enregistrement\n");
                }
                break;
            }



            case 2:  //supprimer un enregistrement
                printf("Entrez l'ID de l'enregistrement à supprimer: ");
                scanf("%d", &id);
                getchar();
                if (supprimerEnregistrement(&fichier, &hashTable, id)) {
                    printf("Enregistrement supprime\n");
                } else {
                    printf("Enregistrement non trouve.\n");
                }
                break;


            case 3: //1st try of rechercherEenregistrement avec le HASH INDEX with a basic if condition
            printf("Entrez l'ID de l'enregistrement à rechercher: ");
            scanf("%d", &id);
            getchar();

            EnregistrementPhysique *trouve = rechercherEnregistrement(&fichier, &hashTable, id);
            if (trouve) {
                printf("enregistrement trouve:\n");
                printf("ID: %d, Data1: %s\n", trouve->entete.id, trouve->data1);
            } else {
                printf("enregistrement non trouve.\n");
            }
            break;


            case 4:
                afficherFichierTOV(&fichier);
                break;



            case 5://quitter
                libererFichierTOV(&fichier);
                return 0;

            default:
                printf("choix non valide. enter again\n");
        

        }

    }
    return 0;
}
