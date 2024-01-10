#include "tov.h"

/*                                                            *  DONE  BY JINX  *                                                        */
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


//liberation du fichier TOv
void libererFichierTOV(FichierTOV *fichier) {
    //verifier si le fichier est null
    if (fichier == NULL) {
        printf("libererFichierTOV: fichier est NULL\n");
        return;
    }
    //si le fichier n'est pas null en le libere et reset de tout les champs concernant les enregistrement
    free(fichier->enregistrements);
    fichier->enregistrements = NULL;
    fichier->entete.nbEnregistrements = 0;

    printf("libererFichierTOV: memoire liberer\n");
}



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

//too much work , explication apres , la fonction marche tres bien pas la peine de la modifier :)
//supprimer un enregistrement specified with his ID du fichier TOV:
bool supprimerEnregistrement(FichierTOV *fichier, int id) {

    const char *nomFichier = "monFichierTOV.tov";
    //cree et open un fichier temporaire pour stocker temporairement les enregistrements qui ne sont pas supprimes.
    const char *nomFichierTemp = "tempFichierTOV.tov";

    if (fichier == NULL) return false;

    FILE *fichierPhysique = fopen(nomFichier, "r");
    FILE *fichierTemp = fopen(nomFichierTemp, "w");

    if (fichierPhysique == NULL || fichierTemp == NULL) {
        if (fichierPhysique != NULL) fclose(fichierPhysique);
        if (fichierTemp != NULL) fclose(fichierTemp);
        return false;
    }

    char ligne[1024];
    EnregistrementPhysique temp;
    bool found = false;
    int indexFound = -1;

    for (int i = 0; fgets(ligne, sizeof(ligne), fichierPhysique) != NULL; ++i) {
        sscanf(ligne, "%d|%s", &temp.entete.id, temp.data1);
        if (temp.entete.id != id) {
            fputs(ligne, fichierTemp);
        } else {
            found = true;
            indexFound = i;
        }
    }

    fclose(fichierPhysique);
    fclose(fichierTemp);

    if (found) {
        //remplace l'ancien fichier par le nouveau
        remove(nomFichier);
        rename(nomFichierTemp, nomFichier);

        //mettre à jour la structure en memoire
        for (int i = indexFound; i < fichier->entete.nbEnregistrements - 1; ++i) {
            fichier->enregistrements[i] = fichier->enregistrements[i + 1];
        }
        fichier->entete.nbEnregistrements--;

        memset(&fichier->enregistrements[fichier->entete.nbEnregistrements], 0, sizeof(EnregistrementPhysique));
    } else {
        remove(nomFichierTemp);
    }

    return found;
}


//fonction pour afficher le contenue de fichier:
void afficherFichierTOV(const FichierTOV *fichier) {
    if (fichier == NULL) return;
    //bouvle simple pour afficher les enregistrement et leur contenue
    printf("Fichier TOV contient %d enregistrements:\n", fichier->entete.nbEnregistrements);
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++) {
        printf("Enregistrement %d:\n", i);
        printf("data1: %s\n", fichier->enregistrements[i].data1);
    }
}


//utiliser le buffer pour gerer le tampon de transmission avec ces 2 fonctions
//Explication later
void remplirBuffer(BufferTransmission *buffer, const char *data) {
    if (buffer == NULL || data == NULL) return;

    strncpy(buffer->data, data, TAILLE_BUFFER);
    buffer->data[TAILLE_BUFFER - 1] = '\0';
    buffer->taille = strlen(buffer->data);
}

void viderBuffer(BufferTransmission *buffer) {
    if (buffer == NULL) return;

    memset(buffer->data, 0, TAILLE_BUFFER);
    buffer->taille = 0;
}


//une fonction pas demandee , j vais l'utiliser juste pour calculer la taille des diff enregistrements
unsigned long CalculerTailleEnregistrement(const EnregistrementPhysique *enregistrement) {
    if (enregistrement == NULL) return 0;

    return sizeof(EnteteEnregistrement) + strlen(enregistrement->data1);
}


//Le programme a un probleme avec le fichier physique , nriglih omba3d
//utiliser les fonctions dans main
int main() {
    //explication des variables
    FichierTOV fichier;
    BufferTransmission buffer;
    int choix, id, capaciteFichier;
    //id sera utiliser dans bcp de fonctions dans le future
    char data[TAILLE_BUFFER];
    EnregistrementPhysique newEnregistrement;

    printf("Entrez la capacité maximale du fichier TOV: ");
    scanf("%d", &capaciteFichier);
    getchar();  //consomme le caractre de nouvelle ligne apr la lecture d'un nombre
    initialiserFichierTOV(&fichier, capaciteFichier);


    while(1)
    {
        printf("\nChoisissez une operation:\n");
        printf("1- Ajouter un enregistrement\n");
        printf("2- Supprimer un enregistrement\n"); //adding it to the switch case;
        printf("3- Calculer la taille d'un enregistrement\n");
        printf("4- Afficher le contenu du fichier\n");
        printf("5- Quitter le programme\n");
        printf("Votre choix: ");
        scanf("%d", &choix);
        getchar();  //consomme le caractere de nouvelle ligne
        
        //1st try of switch case:
        switch(choix)
        {


            case 1:{
                printf("Entrez les données de l'enregistrement:\n");
                printf("data1: ");
                fgets(newEnregistrement.data1, TAILLE_MAX_ENREGISTREMENT, stdin);
                newEnregistrement.data1[strcspn(newEnregistrement.data1, "\n")] = '\0';

                if (ajouterEnregistrement(&fichier, &newEnregistrement)){
                    printf("Enregistrement ajouté avec succès.\n");
                }
                else{
                    printf("Erreur lors de l'ajout de l'enregistrement.\n");
                }
                break;
            }

            case 2:  //supprimer un enregistrement
                printf("Entrez l'ID de l'enregistrement à supprimer: ");
                scanf("%d", &id);
                getchar();
                if (supprimerEnregistrement(&fichier, id)) {
                    printf("Enregistrement supprime\n");
                } else {
                    printf("Enregistrement non trouve.\n");
                }
                break;


            //case 3:  //calculer la taille d'un enregistrement , simple concept mais
                       //je veux la faire avec la fonction de rechercherEnregistrement
               


            case 4:
                afficherFichierTOV(&fichier);
                break;



            case 5: //quitter
                libererFichierTOV(&fichier);
                return 0;

            default:
                printf("choix non valide. enter again\n");
        

        }

    }
    return 0;
}
