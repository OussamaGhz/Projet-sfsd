#include <stdio.h>
#include "tov.h"
#include "string.h"
//1ere fonction inistialisation du fichier
void initialiserFichierTOV(FichierTOV* fichier, int facteurDeBlockage, fichierLogique *fichierLog) {
    //initialisation de l'entete de fichietTOV
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.nbrBlock = 0;
    fichier->entete.facteurdeblockage = facteurDeBlockage;
    fichier->entete.nextId = 0;
    printf(" entete du fichier phisique initialiser avec succes\n");
    // Initialisation du tableau d'enregistrement du fichierTov
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        fichier->enregistrements[i].entete.id = 0;
        fichier->enregistrements[i].entete.tailleDonnees = 0;
        fichier->enregistrements[i].entete.nbrEnregistrementLogique = 0;
        memset(fichier->enregistrements[i].dataBlock, 0, sizeof(fichier->enregistrements[i].dataBlock));
    }
    printf(" fichier phisique initialiser avec succes\n");
// Initialisation du fichierLogique
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        memset(fichierLog->classe[i].nom, 0, sizeof(fichierLog->classe[i].nom));
        for(int j = 0; j < 5; j++) {
            fichierLog->classe[i].prenom[j] = NULL;
        }
        fichierLog->classe[i].matricule = 0;
        memset(fichierLog->classe[i].filiere, 0, sizeof(fichierLog->classe[i].filiere));
    }
    fichierLog->nbrEnregistrementLogique = 0;
printf(" fichier logique initialiser avec succes\n");

}

//fonction de lecture des informations a partir du fichier logique permanent
void chargerDonneesFichierLogique(fichierLogique* fichierLog, const char* cheminFichierLogique) {
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");
  //fichier n'existe pas
    if (fichierLogiqueFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
//fichier existe
    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

   while (i < MAX_ENREGISTREMENTS && !feof(fichierLogiqueFile)) {
    fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom);
    fscanf(fichierLogiqueFile, "prenom %s: %s\n", buffer, enregLog.prenom);
    fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule));
    fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere);

    fichierLog->classe[i] = enregLog;
    i++;

    // Incrémenter nbrEnregistrementLogique une fois pour chaque enregistrement logique complet
    if (i % 4 == 0) {
        fichierLog->nbrEnregistrementLogique++;
    }
}

    fclose(fichierLogiqueFile);
}
//chargement des donnes header
void chargerDonneesFichierTOV(FichierTOV* fichierTOV, const char* cheminHeaderInfo, const char* cheminFichierLogique) {
    FILE* headerInfoFile = fopen(cheminHeaderInfo, "r");
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");
// erreure d'ouverture ou 1 des fichier n'existe pas
    if (headerInfoFile == NULL || fichierLogiqueFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
//les 2 fichier sont ouverts
//lecture de l'entete du fichier backend
 fscanf(headerInfoFile, "FichierTOV Header:\n");
    fscanf(headerInfoFile, "nbEnregistrements: %d\n", &(fichierTOV->entete.nbEnregistrements));
    fscanf(headerInfoFile, "nbrBlock: %d\n", &(fichierTOV->entete.nbrBlock));
    fscanf(headerInfoFile, "facteurdeblockage: %d\n", &(fichierTOV->entete.facteurdeblockage));
    fscanf(headerInfoFile, "nextId: %d\n", &(fichierTOV->entete.nextId));

    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

    while (i < fichierTOV->entete.nbEnregistrements && !feof(headerInfoFile) && !feof(fichierLogiqueFile)) {
        //lecture de l'entete du block i
       fscanf(headerInfoFile, "\nEnregistrementPhysique %d Header:\n", &i);
        fscanf(headerInfoFile, "id: %d\n", &(fichierTOV->enregistrements[i].entete.id));
        fscanf(headerInfoFile, "tailleDonnees: %d\n", &(fichierTOV->enregistrements[i].entete.tailleDonnees));
        fscanf(headerInfoFile, "nbrEnregistrementLogique: %d\n", &(fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique));
        // Lire les données logiques dans le bloc de données i
for (int j = 0; j < fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique; j++) {
    fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom);//lecture du nom

    int k = 0;
    char prenom[20];
    char prenoms[40] = ""; // Pour stocker tous les prénoms (concatenation)

    while (k < 2 && fscanf(fichierLogiqueFile, "prenom %d: %s\n", &k, prenom) == 2) {//si il reussi a lire le prenom
        enregLog.prenom[k-1] = malloc(strlen(prenom) + 1);
        strcpy(enregLog.prenom[k-1], prenom);
        strcat(prenoms, prenom); // Ajouter le prénom à la chaîne des prénoms (concatenation) pour avoire le format phisique souhaite
    }
    fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule));
    fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere);
    
    // Ajouter les données logiques au bloc de données
    sprintf(fichierTOV->enregistrements[i].dataBlock + strlen(fichierTOV->enregistrements[i].dataBlock), "%s||%s||%d||%s||", enregLog.nom,prenoms, enregLog.matricule, enregLog.filiere);
}
        i++;
    }

    fclose(headerInfoFile);
    fclose(fichierLogiqueFile);
}
// fusion des 2 fonction pour que ca soit plus simple
void chargerDonnees(FichierTOV* fichierTOV, fichierLogique* fichierLog, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    chargerDonneesFichierLogique(fichierLog, cheminFichierLogique);
    chargerDonneesFichierTOV(fichierTOV, cheminHeaderInfo,cheminFichierLogique);
}
// verification de resussite de chargement
void afficherDonneesFichierTOV(FichierTOV* fichierTOV) {
    printf("Affichage des données du FichierTOV :\n");
    printf("affichage de l'entete de fichier apres chargement : %d %d %d %d",fichierTOV->entete.nbEnregistrements,fichierTOV->entete.nbrBlock,fichierTOV->entete.facteurdeblockage,fichierTOV->entete.nextId);
    for (int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
        printf("EnregistrementPhysique %d:\n", i);
        printf("  ID: %d\n", fichierTOV->enregistrements[i].entete.id);
        printf("  Taille des données: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
        printf("  Nombre d'enregistrements logiques: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        printf("  Données du bloc: %s\n", fichierTOV->enregistrements[i].dataBlock);
    }
}
void afficherDonneesFichierTOv(FichierTOV* fichierTOV) {
    printf("Affichage des données du FichierTOV :\n");
    printf("affichage de l'entete de fichier apres chargement : %d %d %d %d",fichierTOV->entete.nbEnregistrements,fichierTOV->entete.nbrBlock,fichierTOV->entete.facteurdeblockage,fichierTOV->entete.nextId);
    for (int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
        printf("EnregistrementPhysique %d:\n", i);
        printf("  ID: %d\n", fichierTOV->enregistrements[i].entete.id);
        printf("  Taille des données: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
        printf("  Nombre d'enregistrements logiques: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        printf("  Données du bloc:\n");
        // Parcourir le bloc de données caractère par caractère
        for (int j = 0; j < fichierTOV->enregistrements[i].entete.tailleDonnees; j++) {
            printf("%c", fichierTOV->enregistrements[i].dataBlock[j]);
        }
        printf("\n");
    }
}





   //fonction d'insertion logique et phisique dans un fichier
   void insertionLogique(fichierLogique* fichierLog, FichierTOV* fichierTOV, int nbrInsertion, int facteurDeBlocage, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    
    chargerDonnees(fichierTOV, fichierLog, cheminFichierLogique, cheminHeaderInfo);
    afficherDonneesFichierTOV(fichierTOV);

    int currentBlock = fichierTOV->entete.nbEnregistrements - 1;
    for(int i = 0; i < nbrInsertion; i++) {
        if (fichierLog->nbrEnregistrementLogique >= MAX_ENREGISTREMENTS) {
            printf("le fichierLogique est remplie. on ne peut pas inserer plus d'enregistrement.\n");
            return;
        }
   // la saisie du nouvelle enregistrement
        enregistrementLogique enregLog;

        printf("Enter nom: ");
        scanf("%s", enregLog.nom);
        int n;
        printf("entrer le nombre de vos prenom: ");
        scanf("%d",&n);
        char chaine[40]="";
        for(int j = 0; j < n; j++) {
            enregLog.prenom[j] = malloc(20 * sizeof(char));
            printf("Enter prenom %d: ", j+1);
            scanf("%s", enregLog.prenom[j]);
            strcat(chaine,enregLog.prenom[j]);
        }

        printf("Enter matricule: ");
        scanf("%d", &(enregLog.matricule));

        printf("Enter filiere: ");
        scanf("%s", enregLog.filiere);
       
        fichierLog->classe[fichierLog->nbrEnregistrementLogique] = enregLog;
        fichierLog->nbrEnregistrementLogique++;
        //insertion du nouvelle enregistrement dans le fichier permanent
        FILE* file = fopen(cheminFichierLogique, "a");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            return;
        }

        fprintf(file, "nom: %s\n", enregLog.nom);
        for(int i = 0; i < n; i++) {
            fprintf(file, "prenom %d: %s\n", i+1, enregLog.prenom[i]);
        }
        fprintf(file, "matricule: %d\n", enregLog.matricule);
        fprintf(file, "filiere: %s\n", enregLog.filiere);

        fclose(file);
        //ajoute d'un nouveau block selon le facteur de blockage et la position de current block
        if(currentBlock<0)currentBlock=0;
        if (currentBlock >= fichierTOV->entete.nbEnregistrements || fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique >= facteurDeBlocage) {
            currentBlock = fichierTOV->entete.nbEnregistrements;
            fichierTOV->enregistrements[currentBlock].entete.id = fichierTOV->entete.nextId++;
            fichierTOV->enregistrements[currentBlock].entete.tailleDonnees = 0;
            fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique = 0;
            fichierTOV->entete.nbEnregistrements++;
            fichierTOV->entete.nbrBlock++;
        }
        //copie des element inserer dans le fichier Tov
        char enregLogStr[256];
sprintf(enregLogStr, "%s||%s||%d||%s||", enregLog.nom,chaine, enregLog.matricule, enregLog.filiere);
        memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, enregLogStr,strlen(enregLogStr) + 1);
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += strlen(enregLogStr)+1;
        fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;
        // Write the header information to a file
        file = fopen(cheminHeaderInfo, "w");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            return;
        }

        fprintf(file, "FichierTOV Header:\n");
        fprintf(file, "nbEnregistrements: %d\n", fichierTOV->entete.nbEnregistrements);
        fprintf(file, "nbrBlock: %d\n", fichierTOV->entete.nbrBlock);
        fprintf(file, "facteurdeblockage: %d\n", fichierTOV->entete.facteurdeblockage);
        fprintf(file, "nextId: %d\n", fichierTOV->entete.nextId);

        for(int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
            fprintf(file, "\nEnregistrementPhysique %d Header:\n", i);
            fprintf(file, "id: %d\n", fichierTOV->enregistrements[i].entete.id);
            fprintf(file, "tailleDonnees: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
            fprintf(file, "nbrEnregistrementLogique: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        }

        fclose(file);
    }
    afficherDonneesFichierTOv(fichierTOV);

}

   
bool recherchePhysique(FichierTOV* fichierTOV, int matriculeRecherche) {
    for(int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
        for(int j = 0; j < fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique; j++) {
            enregistrementLogique* enregLog = (enregistrementLogique*)(fichierTOV->enregistrements[i].dataBlock + j * sizeof(enregistrementLogique));
            if (enregLog->matricule == matriculeRecherche) {
                return true;
            }
        }
    }
    return false;
}
//fonction de suppression logique
void supprimerLogique(fichierLogique* fichierLog, int matriculeRecherche) {
    int i;
    for(i = 0; i < fichierLog->nbrEnregistrementLogique; i++) {
        if (fichierLog->classe[i].matricule == matriculeRecherche) {
            break;
        }
    }

    if (i == fichierLog->nbrEnregistrementLogique) {
        printf("L'element avec le matricule %d n'existe pas.\n", matriculeRecherche);
        return;
    }

    for(int j = i; j < fichierLog->nbrEnregistrementLogique - 1; j++) {
        fichierLog->classe[j] = fichierLog->classe[j + 1];
    }

    fichierLog->nbrEnregistrementLogique--;

    printf("L'element avec le matricule %d a ete supprime.\n", matriculeRecherche);
}


    



 









int main(){

    fichierLogique fichier2;
    FichierTOV fichier;
    int facteurDeBlockage;
    initialiserFichierTOV(&fichier,2,&fichier2);
    insertionLogique(&fichier2,&fichier,2,2,"fichierLotest.txt","headerInftest.txt");




}































/* #include <stdio.h>
#include "tov.h"
#include "string.h"
//1ere fonction inistialisation du fichier
void initialiserFichierTOV(FichierTOV* fichier, int facteurDeBlockage, fichierLogique *fichierLog) {
    //initialisation de l'entete de fichietTOV
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.nbrBlock = 0;
    fichier->entete.facteurdeblockage = facteurDeBlockage;
    fichier->entete.nextId = 0;
    printf(" entete du fichier phisique initialiser avec succes\n");
    // Initialisation du tableau d'enregistrement du fichierTov
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        fichier->enregistrements[i].entete.id = 0;
        fichier->enregistrements[i].entete.tailleDonnees = 0;
        fichier->enregistrements[i].entete.nbrEnregistrementLogique = 0;
        memset(fichier->enregistrements[i].dataBlock, 0, sizeof(fichier->enregistrements[i].dataBlock));
    }
    printf(" fichier phisique initialiser avec succes\n");
// Initialisation du fichierLogique
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        memset(fichierLog->classe[i].nom, 0, sizeof(fichierLog->classe[i].nom));
        for(int j = 0; j < 5; j++) {
            fichierLog->classe[i].prenom[j] = NULL;
        }
        fichierLog->classe[i].matricule = 0;
        memset(fichierLog->classe[i].filiere, 0, sizeof(fichierLog->classe[i].filiere));
    }
    fichierLog->nbrEnregistrementLogique = 0;
printf(" fichier logique initialiser avec succes\n");

}

//fonction de lecture des informations a partir du fichier logique permanent
void chargerDonneesFichierLogique(fichierLogique* fichierLog, const char* cheminFichierLogique) {
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");

    if (fichierLogiqueFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier,\n");
        return;
    }

    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

   while (i < MAX_ENREGISTREMENTS && !feof(fichierLogiqueFile)) {
    fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom);
    fscanf(fichierLogiqueFile, "prenom %s: %s\n", buffer, enregLog.prenom);
    fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule));
    fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere);

    fichierLog->classe[i] = enregLog;
    i++;

    // Incrémenter nbrEnregistrementLogique une fois pour chaque enregistrement logique complet
    if (i % 4 == 0) {
        fichierLog->nbrEnregistrementLogique++;
    }
}

    fclose(fichierLogiqueFile);
}
void chargerDonneesFichierTOV(FichierTOV* fichierTOV, const char* cheminHeaderInfo, const char* cheminFichierLogique) {
    FILE* headerInfoFile = fopen(cheminHeaderInfo, "r");
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");

    if (headerInfoFile == NULL || fichierLogiqueFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

 fscanf(headerInfoFile, "FichierTOV Header:\n");
    fscanf(headerInfoFile, "nbEnregistrements: %d\n", &(fichierTOV->entete.nbEnregistrements));
    fscanf(headerInfoFile, "nbrBlock: %d\n", &(fichierTOV->entete.nbrBlock));
    fscanf(headerInfoFile, "facteurdeblockage: %d\n", &(fichierTOV->entete.facteurdeblockage));
    fscanf(headerInfoFile, "nextId: %d\n", &(fichierTOV->entete.nextId));

    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

    while (i < fichierTOV->entete.nbEnregistrements && !feof(headerInfoFile)) {
        // ... (lire l'entête de l'enregistrement physique comme avant) ...
       if (fscanf(headerInfoFile, "\nEnregistrementPhysique %d Header:\n", &i) != 1) break;
    if (fscanf(headerInfoFile, "id: %d\n", &(fichierTOV->enregistrements[i].entete.id)) != 1) break;
    if (fscanf(headerInfoFile, "tailleDonnees: %d\n", &(fichierTOV->enregistrements[i].entete.tailleDonnees)) != 1) break;
    if (fscanf(headerInfoFile, "nbrEnregistrementLogique: %d\n", &(fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique)) != 1) break;
        // Lire les données logiques dans le bloc de données
        
for (int j = 0; j < fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique; j++) {
    if (fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom) != 1) break;

    int k = 0;
    char prenom[20];
    char prenoms[40] = ""; // Pour stocker tous les prénoms

    while (k < 2 && fscanf(fichierLogiqueFile, "prenom %d: %s\n", &k, prenom) == 2) {
        enregLog.prenom[k-1] = malloc(strlen(prenom) + 1);
        strcpy(enregLog.prenom[k-1], prenom);
        strcat(prenoms, prenom); // Ajouter le prénom à la chaîne des prénoms
    }

    if (fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule)) != 1) break;
        if (fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere) != 1) break;
    
    // Ajouter les données logiques au bloc de données
    sprintf(fichierTOV->enregistrements[i].dataBlock + strlen(fichierTOV->enregistrements[i].dataBlock), "%s||%s||%d||%s||", enregLog.nom,prenoms, enregLog.matricule, enregLog.filiere);
}
        i++;
    }

    fclose(headerInfoFile);
    fclose(fichierLogiqueFile);
}
// fusion des 2 fonction pour que ca soit plus simple
void chargerDonnees(FichierTOV* fichierTOV, fichierLogique* fichierLog, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    chargerDonneesFichierLogique(fichierLog, cheminFichierLogique);
    chargerDonneesFichierTOV(fichierTOV, cheminHeaderInfo,cheminFichierLogique);
}
// verification de resussite de chargement
void afficherDonneesFichierTOV(FichierTOV* fichierTOV) {
    printf("Affichage des données du FichierTOV :\n");
    for (int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
        printf("EnregistrementPhysique %d:\n", i);
        printf("  ID: %d\n", fichierTOV->enregistrements[i].entete.id);
        printf("  Taille des données: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
        printf("  Nombre d'enregistrements logiques: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        printf("  Données du bloc: %s\n", fichierTOV->enregistrements[i].dataBlock);
    }
}

   //fonction d'insertion logique et phisique dans un fichier
   void insertionLogique(fichierLogique* fichierLog, FichierTOV* fichierTOV, int nbrInsertion, int facteurDeBlocage, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    //chargement des donne des 2fichier permanents
    chargerDonnees(fichierTOV, fichierLog, cheminFichierLogique, cheminHeaderInfo);
    afficherDonneesFichierTOV(fichierTOV);//affichage pour la verification du chargement
// entrer des information logique / les information a inserer
    int currentBlock = fichierTOV->entete.nbEnregistrements - 1;
    for(int i = 0; i < nbrInsertion; i++) {
        if (fichierLog->nbrEnregistrementLogique >= MAX_ENREGISTREMENTS) {
            printf("le fichierLogique est remplie. on ne peut pas inserer plus d'enregistrement.\n");
            return;
        }
   
        enregistrementLogique enregLog;

        printf("Enter nom: ");
        scanf("%s", enregLog.nom);
        int n;
        printf("entrer le nombre de vos prenom: ");
        scanf("%d",&n);
        char chaine[20];
        for(int j = 0; j < n; j++) {
            enregLog.prenom[j] = malloc(20 * sizeof(char));
            printf("Enter prenom %d: ", j+1);
            scanf("%s", enregLog.prenom[j]);
        }
        
        printf("Enter matricule: ");
        scanf("%d", &(enregLog.matricule));

        printf("Enter filiere: ");
        scanf("%s", enregLog.filiere);
// ajout des information logique dans la structure temporaire
        fichierLog->classe[fichierLog->nbrEnregistrementLogique] = enregLog;
        fichierLog->nbrEnregistrementLogique++;
//ajout des nouvelle information dans le fichier de donne permanent 
        FILE* file = fopen(cheminFichierLogique, "a");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            return;
        }

        fprintf(file, "nom: %s\n", enregLog.nom);
        for(int i = 0; i < n; i++) {
            fprintf(file, "prenom %d: %s\n", i+1, enregLog.prenom[i]);
        }
        fprintf(file, "matricule: %d\n", enregLog.matricule);
        fprintf(file, "filiere: %s\n", enregLog.filiere);

        fclose(file);
// ajout des information phisique
//ajout de nouveau block/ cas ou le block est plein ou n'existe pas
        if (currentBlock >= fichierTOV->entete.nbEnregistrements || fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique >= facteurDeBlocage) {
            currentBlock = fichierTOV->entete.nbEnregistrements;
            fichierTOV->enregistrements[currentBlock].entete.id = fichierTOV->entete.nextId++;
            fichierTOV->enregistrements[currentBlock].entete.tailleDonnees = 0;
            fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique = 0;
            fichierTOV->entete.nbEnregistrements++;
            fichierTOV->entete.nbrBlock++;
        }  
//copie des nouvelle donne a l'aide  du buffer
 memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, &enregLog, sizeof(enregistrementLogique));
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += sizeof(enregistrementLogique);
        fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;
/*char enregLogStr[256];
sprintf(enregLogStr, "%s||%s||%s||%d||%s||", enregLog.nom, enregLog.prenom[0], enregLog.prenom[1], enregLog.matricule, enregLog.filiere);
        memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, enregLogStr,strlen(enregLogStr) + 1);
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += strlen(enregLogStr)+1;
        fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;
//actualisation du fichier header
        file = fopen(cheminHeaderInfo, "w");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            return;
        }

        fprintf(file, "FichierTOV Header:\n");
        fprintf(file, "nbEnregistrements: %d\n", fichierTOV->entete.nbEnregistrements);
        fprintf(file, "nbrBlock: %d\n", fichierTOV->entete.nbrBlock);
        fprintf(file, "facteurdeblockage: %d\n", fichierTOV->entete.facteurdeblockage);
        fprintf(file, "nextId: %d\n", fichierTOV->entete.nextId);

        for(int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
            fprintf(file, "\nEnregistrementPhysique %d Header:\n", i);
            fprintf(file, "id: %d\n", fichierTOV->enregistrements[i].entete.id);
            fprintf(file, "tailleDonnees: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
            fprintf(file, "nbrEnregistrementLogique: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        }

        fclose(file);
    }    afficherDonneesFichierTOV(fichierTOV);

        memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, &enregLog, sizeof(enregistrementLogique));
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += sizeof(enregistrementLogique);
        fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;



}












   
bool recherchePhysique(FichierTOV* fichierTOV, int matriculeRecherche) {
    for(int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
        for(int j = 0; j < fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique; j++) {
            enregistrementLogique* enregLog = (enregistrementLogique*)(fichierTOV->enregistrements[i].dataBlock + j * sizeof(enregistrementLogique));
            if (enregLog->matricule == matriculeRecherche) {
                return true;
            }
        }
    }
    return false;
}
//fonction de suppression logique
void supprimerLogique(fichierLogique* fichierLog, int matriculeRecherche) {
    int i;
    for(i = 0; i < fichierLog->nbrEnregistrementLogique; i++) {
        if (fichierLog->classe[i].matricule == matriculeRecherche) {
            break;
        }
    }

    if (i == fichierLog->nbrEnregistrementLogique) {
        printf("L'element avec le matricule %d n'existe pas.\n", matriculeRecherche);
        return;
    }

    for(int j = i; j < fichierLog->nbrEnregistrementLogique - 1; j++) {
        fichierLog->classe[j] = fichierLog->classe[j + 1];
    }

    fichierLog->nbrEnregistrementLogique--;

    printf("L'element avec le matricule %d a ete supprime.\n", matriculeRecherche);
}


    



 









int main(){

    fichierLogique fichier2;
    FichierTOV fichier;
    int facteurDeBlockage;
    initialiserFichierTOV(&fichier,2,&fichier2);
    insertionLogique(&fichier2,&fichier,2,2,"fichierLogiquetest.txt","headerInfotest.txt");




}

*/




















/* 1ere fonction inistialisation du fichier
void initialiserFichierTOV(FichierTOV* fichier, int facteurDeBlockage, fichierLogique *fichierLog) {
    //initialisation de l'entete de fichietTOV
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.nbrBlock = 0;
    fichier->entete.facteurdeblockage = facteurDeBlockage;
    fichier->entete.nextId = 0;
    printf(" entete du fichier phisique initialiser avec succes\n");
    // Initialisation du tableau d'enregistrement du fichierTov
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        fichier->enregistrements[i].entete.id = 0;
        fichier->enregistrements[i].entete.tailleDonnees = 0;
        fichier->enregistrements[i].entete.nbrEnregistrementLogique = 0;
        memset(fichier->enregistrements[i].dataBlock, 0, sizeof(fichier->enregistrements[i].dataBlock));
    }
    printf(" fichier phisique initialiser avec succes\n");
// Initialisation du fichierLogique
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        memset(fichierLog->classe[i].nom, 0, sizeof(fichierLog->classe[i].nom));
        for(int j = 0; j < 5; j++) {
            fichierLog->classe[i].prenom[j] = NULL;
        }
        fichierLog->classe[i].matricule = 0;
        memset(fichierLog->classe[i].filiere, 0, sizeof(fichierLog->classe[i].filiere));
    }
    fichierLog->nbrEnregistrementLogique = 0;
printf(" fichier logique initialiser avec succes\n");

}
//fonction de lecture des informations a partir du fichier logique permanent
void chargerDonneesFichierLogique(fichierLogique* fichierLog, const char* cheminFichierLogique) {
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");

    if (fichierLogiqueFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

   while (i < MAX_ENREGISTREMENTS && !feof(fichierLogiqueFile)) {
    fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom);
    fscanf(fichierLogiqueFile, "prenom %s: %s\n", buffer, enregLog.prenom);
    fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule));
    fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere);

    fichierLog->classe[i] = enregLog;
    i++;

    // Incrémenter nbrEnregistrementLogique une fois pour chaque enregistrement logique complet
    if (i % 4 == 0) {
        fichierLog->nbrEnregistrementLogique++;
    }
}

    fclose(fichierLogiqueFile);
}
// fonction de lecture des info a partir du fichier phisique permanent
void chargerDonneesFichierTOV(FichierTOV* fichierTOV, const char* cheminHeaderInfo) {
    FILE* headerInfoFile = fopen(cheminHeaderInfo, "r");

    if (headerInfoFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    char buffer[50];
    int i = 0;

    fscanf(headerInfoFile, "FichierTOV Header:\n");
    fscanf(headerInfoFile, "nbEnregistrements: %d\n", &(fichierTOV->entete.nbEnregistrements));
    fscanf(headerInfoFile, "nbrBlock: %d\n", &(fichierTOV->entete.nbrBlock));
    fscanf(headerInfoFile, "facteurdeblockage: %d\n", &(fichierTOV->entete.facteurdeblockage));
    fscanf(headerInfoFile, "nextId: %d\n", &(fichierTOV->entete.nextId));

    while (i < fichierTOV->entete.nbEnregistrements && !feof(headerInfoFile)) {
        fscanf(headerInfoFile, "\nEnregistrementPhysique %d Header:\n", &i);
        fscanf(headerInfoFile, "id: %d\n", &(fichierTOV->enregistrements[i].entete.id));
        fscanf(headerInfoFile, "tailleDonnees: %d\n", &(fichierTOV->enregistrements[i].entete.tailleDonnees));
        fscanf(headerInfoFile, "nbrEnregistrementLogique: %d\n", &(fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique));
        i++;
    }

    fclose(headerInfoFile);
}
// fusion des 2 fonction pour que ca soit plus simple
void chargerDonnees(FichierTOV* fichierTOV, fichierLogique* fichierLog, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    chargerDonneesFichierLogique(fichierLog, cheminFichierLogique);
    chargerDonneesFichierTOV(fichierTOV, cheminHeaderInfo);
}

   //fonction d'insertion logique et phisique dans un fichier
   void insertionLogique(fichierLogique* fichierLog, FichierTOV* fichierTOV, int nbrInsertion, int facteurDeBlocage, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    chargerDonnees(fichierTOV, fichierLog, cheminFichierLogique, cheminHeaderInfo);
    int currentBlock = fichierTOV->entete.nbEnregistrements - 1;
    for(int i = 0; i < nbrInsertion; i++) {
        if (fichierLog->nbrEnregistrementLogique >= MAX_ENREGISTREMENTS) {
            printf("le fichierLogique est remplie. on ne peut pas inserer plus d'enregistrement.\n");
            return;
        }

        enregistrementLogique enregLog;

        printf("Enter nom: ");
        scanf("%s", enregLog.nom);
        int n;
        printf("entrer le nombre de vos prenom: ");
        scanf("%d",&n);

        for(int j = 0; j < n; j++) {
            enregLog.prenom[j] = malloc(20 * sizeof(char));
            printf("Enter prenom %d: ", j+1);
            scanf("%s", enregLog.prenom[j]);
        }

        printf("Enter matricule: ");
        scanf("%d", &(enregLog.matricule));

        printf("Enter filiere: ");
        scanf("%s", enregLog.filiere);

        fichierLog->classe[fichierLog->nbrEnregistrementLogique] = enregLog;
        fichierLog->nbrEnregistrementLogique++;

        FILE* file = fopen("fichierLogique2.txt", "a");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            return;
        }

        fprintf(file, "nom: %s\n", enregLog.nom);
        for(int i = 0; i < n; i++) {
            fprintf(file, "prenom %d: %s\n", i+1, enregLog.prenom[i]);
        }
        fprintf(file, "matricule: %d\n", enregLog.matricule);
        fprintf(file, "filiere: %s\n", enregLog.filiere);

        fclose(file);

        if (currentBlock >= fichierTOV->entete.nbEnregistrements || fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique >= facteurDeBlocage) {
            currentBlock = fichierTOV->entete.nbEnregistrements;
            fichierTOV->enregistrements[currentBlock].entete.id = fichierTOV->entete.nextId++;
            fichierTOV->enregistrements[currentBlock].entete.tailleDonnees = 0;
            fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique = 0;
            fichierTOV->entete.nbEnregistrements++;
            fichierTOV->entete.nbrBlock++;
        }

        memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, &enregLog, sizeof(enregistrementLogique));
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += sizeof(enregistrementLogique);
        fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;

        // Write the header information to a file
        file = fopen("headerInfo.txt", "w");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            return;
        }

        fprintf(file, "FichierTOV Header:\n");
        fprintf(file, "nbEnregistrements: %d\n", fichierTOV->entete.nbEnregistrements);
        fprintf(file, "nbrBlock: %d\n", fichierTOV->entete.nbrBlock);
        fprintf(file, "facteurdeblockage: %d\n", fichierTOV->entete.facteurdeblockage);
        fprintf(file, "nextId: %d\n", fichierTOV->entete.nextId);

        for(int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
            fprintf(file, "\nEnregistrementPhysique %d Header:\n", i);
            fprintf(file, "id: %d\n", fichierTOV->enregistrements[i].entete.id);
            fprintf(file, "tailleDonnees: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
            fprintf(file, "nbrEnregistrementLogique: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        }

        fclose(file);
    }
}

   
bool recherchePhysique(FichierTOV* fichierTOV, int matriculeRecherche) {
    for(int i = 0; i < fichierTOV->entete.nbEnregistrements; i++) {
        for(int j = 0; j < fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique; j++) {
            enregistrementLogique* enregLog = (enregistrementLogique*)(fichierTOV->enregistrements[i].dataBlock + j * sizeof(enregistrementLogique));
            if (enregLog->matricule == matriculeRecherche) {
                return true;
            }
        }
    }
    return false;
}
//fonction de suppression logique
void supprimerLogique(fichierLogique* fichierLog, int matriculeRecherche) {
    int i;
    for(i = 0; i < fichierLog->nbrEnregistrementLogique; i++) {
        if (fichierLog->classe[i].matricule == matriculeRecherche) {
            break;
        }
    }

    if (i == fichierLog->nbrEnregistrementLogique) {
        printf("L'element avec le matricule %d n'existe pas.\n", matriculeRecherche);
        return;
    }

    for(int j = i; j < fichierLog->nbrEnregistrementLogique - 1; j++) {
        fichierLog->classe[j] = fichierLog->classe[j + 1];
    }

    fichierLog->nbrEnregistrementLogique--;

    printf("L'element avec le matricule %d a ete supprime.\n", matriculeRecherche);
}


    



 









int main(){

    fichierLogique fichier2;
    FichierTOV fichier;
    int facteurDeBlockage;
    initialiserFichierTOV(&fichier,2,&fichier2);
    insertionLogique(&fichier2,&fichier,1,2,"fichierLogique2.txt","headerInfo.txt");


}
*/