#include <stdio.h>
#include "tov.h"
#include "string.h"
     //1ere fonction inistialisation du fichier
void initialiserFichierTOV(FichierTOV* fichier, int facteurDeBlockage, fichierLogique *fichierLog) {
    //initialisation de l'entete de fichietTOV
    fichier->entete.nbEnregistrementslogique = 0;
    fichier->entete.nbrBlock = 0;
    fichier->entete.facteurdeblockage = facteurDeBlockage;
    fichier->entete.nextId = 0;
    printf(" entete du fichier phisique initialiser avec succes\n");

    // Initialisation du tableau d'enregistrement du fichierTov
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        fichier->enregistrements[i].entete.id = 0;
        fichier->enregistrements[i].entete.tailleDonnees = 0;
        fichier->enregistrements[i].entete.nbrEnregistrementLogique = 0;
        memset(fichier->enregistrements[i].dataBlock, 0, sizeof(fichier->enregistrements[i].dataBlock));//initialiser chaque octet de datablock avec 0 
    }
    printf(" fichier phisique initialiser avec succes\n");

// Initialisation du fichierLogique
    for(int i = 0; i < MAX_ENREGISTREMENTS; i++) {
        memset(fichierLog->classe[i].nom, 0, sizeof(fichierLog->classe[i].nom));
        for(int j = 0; j < 5; j++) {
            fichierLog->classe[i].prenom[j] = NULL;
        }
        fichierLog->classe[i].matricule = 0;
        fichierLog->classe[i].suplogique = 0;
        fichierLog->classe[i].id=-1;
        fichierLog->classe[i].taille=0;
        memset(fichierLog->classe[i].filiere, 0, sizeof(fichierLog->classe[i].filiere));
    }
      fichierLog->nbrEnregistrementLogique = 0;
      printf(" fichier logique initialiser avec succes\n");

}
  //initialisation de la table hachable
HashTable* initialiserHachable() {
    HashTable* table = malloc(sizeof(HashTable));
    for (int i = 0; i < SIZE; i++) {
        table->tab[i] = NULL;
    }
    return table;
}
//function hachable pour retrouver l'index de l'elelement rechercher
int positiontab(int cle) {
    return cle % SIZE;
}
//function d'insertion dans la structure hachable el LIFO
 void insertionhachable(HashTable* table, int cle,int idBlock) {
    int index = positiontab(cle);
    ElementTablHachabl* newelement = malloc(sizeof(ElementTablHachabl));
    newelement->id = cle;
    newelement->idBlock = idBlock;
    newelement->next = table->tab[index];
    table->tab[index] = newelement;
}
//function de recherche d'un element dans la structure hachable et qui retourne sa case  
ElementTablHachabl* rechercherElement(HashTable* table, int cle) {
    
    int index = positiontab(cle);
    ElementTablHachabl* courant = table->tab[index];
    while (courant != NULL) {
        if (courant->id == cle) {
            return courant;
        }
        courant = courant->next;
    }
    return NULL;
}
//function d'affichage de notre tableau
void afficherTableHachage(HashTable* table) {
    for (int i = 0; i < SIZE; i++) {
        ElementTablHachabl* courant = table->tab[i];
        printf("Index %d:\n", i);
        while (courant != NULL) {
            printf("  Clé: %d, ID du bloc: %d\n",courant->id, courant->idBlock);
            courant = courant->next;
        }
    }
}
//function d ecgargement de donne apres la fin du traitement (stockage de donne)
void chargementFinal(FichierTOV* fichierTOV, const char* cheminFichierLogique) {
    FILE* fichierLogique = fopen(cheminFichierLogique, "w");
    if (fichierLogique == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    for (int i = 0; i < fichierTOV->entete.nbrBlock; i++) {

        EnregistrementPhysique* block = &(fichierTOV->enregistrements[i]);//recuperer le i eme block
        // recuperer tous les enregistrement logique du i eme block
        char* ptr = block->dataBlock;//pointe sur le premier caractere

        while (ptr < block->dataBlock + block->entete.tailleDonnees) {
            int tailleEnregLog, etatSuppression, idEnregLog;
            char nom[20], prenoms[110], filiere[20];
            int matricule;
            //recuperer les donne de l'enregistrement
            sscanf(ptr, "%d||%d||%d||%[^|]||%[^|]||%d||%[^|]", &tailleEnregLog, &etatSuppression, &idEnregLog, nom, prenoms, &matricule, filiere);
            
            //ecrire les information dans le fichier permanent en gardant le format suivie
            fprintf(fichierLogique, "id: %d\n", idEnregLog);
            fprintf(fichierLogique, "nom: %s\n", nom);
             char* prenomst = prenoms; // Replace with your string of first names
             char* prenom;
                int k = 1;
                // Use strtok to split the string into first names
                prenom = strtok(prenoms, "$");
                while (prenom != NULL) {
                    fprintf(fichierLogique, "prenom %d: %s\n", k, prenom);
                    prenom = strtok(NULL, "$");
                    k++;
                }
            fprintf(fichierLogique, "matricule: %d\n", matricule);
            fprintf(fichierLogique, "filiere: %s\n", filiere);
            fprintf(fichierLogique, "supprimerLogiquement: %d\n", etatSuppression);
            fprintf(fichierLogique, "tailleDonneEnregistrement: %d\n", tailleEnregLog);

            ptr += tailleEnregLog; // aller au prochaine enregistrement logique
        }
    }

    fclose(fichierLogique);
}
// function de suppression d'un element dans la table hachable
void supprimerElement(HashTable* table, int cle) {

    int index = positiontab(cle);
    ElementTablHachabl* courant = table->tab[index];
    ElementTablHachabl* precedent = NULL;

    while (courant != NULL) {
        if (courant->id == cle) {
            if (precedent == NULL) {
                table->tab[index] = courant->next;
            } else {
                precedent->next = courant->next;
            }
            free(courant);
            return;
        }
        precedent = courant;
        courant = courant->next;
    }
}
// function pour modifier le champ idblock d'un element 
void modifierIdBlock(HashTable* table, int cle, int nouveauIdBlock) {
    ElementTablHachabl* element = rechercherElement(table, cle);
    if (element != NULL) {
        element->idBlock = nouveauIdBlock;
    }
}
//fonction de stockage des entete dans la structure permanente
void ecrireEnteteFichierTOV(FichierTOV* fichierTOV, const char* cheminHeaderInfo) {

    FILE* file = fopen(cheminHeaderInfo, "w");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    // ecrire les entetes de fichier avec le format suivie
    fprintf(file, "FichierTOV Header:\n");
    fprintf(file, "nbEnregistrements: %d\n", fichierTOV->entete.nbEnregistrementslogique);
    fprintf(file, "nbrBlock: %d\n", fichierTOV->entete.nbrBlock);
    fprintf(file, "facteurdeblockage: %d\n", fichierTOV->entete.facteurdeblockage);
    fprintf(file, "nextId: %d\n", fichierTOV->entete.nextId);

    // ecrire les entetes des block avec le format suivie
    for(int i = 0; i < fichierTOV->entete.nbrBlock; i++) {
        fprintf(file, "\nEnregistrementPhysique %d Header:\n", i);
        fprintf(file, "id: %d\n", fichierTOV->enregistrements[i].entete.id);
        fprintf(file, "tailleDonnees: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
        fprintf(file, "nbrEnregistrementLogique: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
    }

    fclose(file);
}

//fonction de lecture des informations a partir du fichier logique permanent
void chargerDonneesFichierLogique(fichierLogique* fichierLog, const char* cheminFichierLogique) {
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");
  // si fichier n'existe pas
    if (fichierLogiqueFile == NULL) {
        printf("votre fichier n'existe pas.\n");
        return;
    }
//fichier existe
    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

   while (i < MAX_ENREGISTREMENTS && !feof(fichierLogiqueFile)) {

    fscanf(fichierLogiqueFile, "id: %d\n", &(enregLog.id));
    fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom);
    fscanf(fichierLogiqueFile, "prenom %s: %s\n", buffer, enregLog.prenom);
    fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule));
    fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere);
    fscanf(fichierLogiqueFile, "supprimerLogiquement: %d\n", &(enregLog.suplogique));
    fscanf(fichierLogiqueFile, "tailleDonneEnregistrement: %d\n", &(enregLog.taille));
    fichierLog->classe[i] = enregLog;
    i++;

    // Incrémenter nbrEnregistrementLogique une fois pour chaque enregistrement logique complet
    if (i % 4 == 0) {
        fichierLog->nbrEnregistrementLogique++;
    }
}

    fclose(fichierLogiqueFile);
}

//chargement des donnes header backend + donnes logique dans la structure TOV
void chargerDonneesFichierTOV(HashTable* table,FichierTOV* fichierTOV, const char* cheminHeaderInfo, const char* cheminFichierLogique) {
    FILE* headerInfoFile = fopen(cheminHeaderInfo, "r");
    FILE* fichierLogiqueFile = fopen(cheminFichierLogique, "r");

// erreure d'ouverture ou 1 des fichier n'existe pas
    if (headerInfoFile == NULL || fichierLogiqueFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier ou vos fichier n'existe pas.\n");
        return;
    }

//les 2 fichier sont ouverts
//lecture de l'entete du fichier backend selon le format suivie
 fscanf(headerInfoFile, "FichierTOV Header:\n");
    fscanf(headerInfoFile, "nbEnregistrements: %d\n", &(fichierTOV->entete.nbEnregistrementslogique));
    fscanf(headerInfoFile, "nbrBlock: %d\n", &(fichierTOV->entete.nbrBlock));
    fscanf(headerInfoFile, "facteurdeblockage: %d\n", &(fichierTOV->entete.facteurdeblockage));
    fscanf(headerInfoFile, "nextId: %d\n", &(fichierTOV->entete.nextId));

    enregistrementLogique enregLog;
    int i = 0;
    char buffer[20];

    while (i < fichierTOV->entete.nbrBlock && !feof(headerInfoFile) && !feof(fichierLogiqueFile)) {
        //lecture de l'entete du block i
       fscanf(headerInfoFile, "\nEnregistrementPhysique %d Header:\n", &i);
        fscanf(headerInfoFile, "id: %d\n", &(fichierTOV->enregistrements[i].entete.id));
        fscanf(headerInfoFile, "tailleDonnees: %d\n", &(fichierTOV->enregistrements[i].entete.tailleDonnees));
        fscanf(headerInfoFile, "nbrEnregistrementLogique: %d\n", &(fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique));
        // Lire les données logiques dans le bloc de données i
for (int j = 0; j < fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique; j++) {

    fscanf(fichierLogiqueFile, "id: %d\n", &(enregLog.id));//lecture de l'id de l'etudiant
    fscanf(fichierLogiqueFile, "nom: %s\n", enregLog.nom);//lecture du nom
    insertionhachable(table,enregLog.id,fichierTOV->enregistrements[i].entete.id);//insertion de l'element dans la structure hachable
    int k = 0;
    char prenom[20];
    char prenoms[110] = ""; // Pour stocker tous les prénoms (concatenation)
    int b;
    while (k < 5 && fscanf(fichierLogiqueFile, "prenom %d: %s\n", &b, prenom) == 2) {//si il reussi a lire le prenom
        enregLog.prenom[k] = malloc(strlen(prenom) + 1);
        strcpy(enregLog.prenom[k], prenom);
        strcat(prenoms, prenom); // Ajouter le prénom à la chaîne des prénoms (concatenation) pour avoire le format phisique souhaite
       // separer les prenom avec un $
        strcat(prenoms, "$");
    
    k++;
    }
    // Remove the last character from prenoms if it's a '$'
    int len = strlen(prenoms);
    if (prenoms[len - 1] == '$') {
        prenoms[len - 1] = '\0';
    }

    fscanf(fichierLogiqueFile, "matricule: %d\n", &(enregLog.matricule));
    fscanf(fichierLogiqueFile, "filiere: %s\n", enregLog.filiere);
    fscanf(fichierLogiqueFile, "supprimerLogiquement: %d\n", &(enregLog.suplogique));
    fscanf(fichierLogiqueFile, "tailleDonneEnregistrement: %d\n", &(enregLog.taille));
    
    // Ajouter les données logiques au bloc de données en suivant le format 
    sprintf(fichierTOV->enregistrements[i].dataBlock + strlen(fichierTOV->enregistrements[i].dataBlock), "%d||%d||%d||%s||%s||%d||%s||",enregLog.taille,enregLog.suplogique,enregLog.id, enregLog.nom,prenoms, enregLog.matricule, enregLog.filiere);
}
        i++;
    }

    fclose(headerInfoFile);
    fclose(fichierLogiqueFile);
}

// fusion des 2 fonction pour que ca soit plus simple
void chargerDonnees(HashTable* table,FichierTOV* fichierTOV, fichierLogique* fichierLog, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    chargerDonneesFichierLogique(fichierLog, cheminFichierLogique);
    chargerDonneesFichierTOV(table,fichierTOV, cheminHeaderInfo,cheminFichierLogique);
}

// verification de resussite de chargement en affichant les entetes de la structure tov
void afficherDonneesFichierTOV(FichierTOV* fichierTOV) {
    printf("Affichage des données du FichierTOV :\n");
    printf("affichage de l'entete de fichier apres chargement : %d %d %d %d\n",fichierTOV->entete.nbEnregistrementslogique,fichierTOV->entete.nbrBlock,fichierTOV->entete.facteurdeblockage,fichierTOV->entete.nextId);
    for (int i = 0; i < fichierTOV->entete.nbrBlock; i++) {
        printf("EnregistrementPhysique %d:\n", i);
        printf("  ID: %d\n", fichierTOV->enregistrements[i].entete.id);
        printf("  Taille des données: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
        printf("  Nombre d'enregistrements logiques: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        printf("  Données du bloc: %s\n", fichierTOV->enregistrements[i].dataBlock);
    }
}//verification de reussite de l'insertion en affichant la structure tov completement
void afficherDonneesFichierTOv(FichierTOV* fichierTOV) {
    printf("Affichage des données du FichierTOV :\n");
    printf("affichage de l'entete de fichier apres chargement : %d %d %d %d\n",fichierTOV->entete.nbEnregistrementslogique,fichierTOV->entete.nbrBlock,fichierTOV->entete.facteurdeblockage,fichierTOV->entete.nextId);
    for (int i = 0; i < fichierTOV->entete.nbrBlock; i++) {
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
   void insertionLogique(HashTable* table,fichierLogique* fichierLog, FichierTOV* fichierTOV, int nbrInsertion, int facteurDeBlocage, const char* cheminFichierLogique, const char* cheminHeaderInfo) {
    //chargement des donne +affichage des donne charge
    chargerDonnees(table,fichierTOV, fichierLog, cheminFichierLogique, cheminHeaderInfo);
    afficherDonneesFichierTOV(fichierTOV);
    afficherTableHachage(table);
    int currentBlock=0;
 if(fichierTOV->entete.nbrBlock!=0){// on se postione au dernier block
     currentBlock=fichierTOV->entete.nbrBlock-1;
    }
    for(int i = 0; i < nbrInsertion; i++) { //verification de la capacite max
        if (fichierLog->nbrEnregistrementLogique >= MAX_ENREGISTREMENTS) {
            printf("le fichierLogique est remplie. on ne peut pas inserer plus d'enregistrement.\n");
            return;
        }
     // la saisie du nouvelle enregistrement a inserer
        enregistrementLogique enregLog;
        
        enregLog.id=fichierTOV->entete.nbEnregistrementslogique;
        enregLog.suplogique=0;                          //determination de l'id de chaque etudiant automatiquement
        printf("Enter nom: ");
        scanf("%s", enregLog.nom);
        
        int n;
        printf("entrer le nombre de vos prenom: ");
        scanf("%d",&n); //max 5 jusqu'a maintenant selon le format
        char chaine[110]="";//pour la concatenation (besois dans le stockage de data)
         
         for(int j = 0; j < n; j++) {
    enregLog.prenom[j] = malloc(20 * sizeof(char));
    printf("Enter prenom %d: ", j+1);
    scanf("%s", enregLog.prenom[j]);//insertion des prenom
    strcat(chaine, enregLog.prenom[j]);//concatenation des prenoms dans chaine pour avoire le format souhaite
    if (j < n - 1) {// separer les prenom avec un |
        strcat(chaine, "$");
    }
}
        printf("Enter matricule: ");
        scanf("%d", &(enregLog.matricule));

        printf("Enter filiere: ");
        scanf("%s", enregLog.filiere);

        char enregLogTov[256];//suport de stockage contenant le format de data souhaite on le considere comme etant le buffer
        sprintf(enregLogTov, "%d||%d||%s||%s||%d||%s||",enregLog.suplogique,enregLog.id,enregLog.nom,chaine, enregLog.matricule, enregLog.filiere);//copie des information pour avoire le format souhaite
        int tailleenrgistrement=strlen(enregLogTov);
        char enreglog2[20];//pour la transition
        sprintf(enreglog2,"%d||",tailleenrgistrement);
        tailleenrgistrement+=strlen(enreglog2); 
       //stockage dans la structure fichierlog (stockage tamporaire)
        fichierLog->classe[fichierLog->nbrEnregistrementLogique] = enregLog;
        fichierLog->nbrEnregistrementLogique++;
        fichierTOV->entete.nbEnregistrementslogique++;
        //insertion du nouvelle enregistrement dans le fichier permanent(stockage permanent)
        FILE* file = fopen(cheminFichierLogique, "a");//mode ajouter
        if (file == NULL) {
            printf("echece d'ouverture du fichier.\n");
            return;
        }
        //l'ecrirure des donne inserer dans la fin du fichier
        fprintf(file, "id: %d\n", enregLog.id);
        fprintf(file, "nom: %s\n", enregLog.nom);

        for(int i = 0; i < n; i++) {
            fprintf(file, "prenom %d: %s\n", i+1, enregLog.prenom[i]);
        }

        fprintf(file, "matricule: %d\n", enregLog.matricule);
        fprintf(file, "filiere: %s\n", enregLog.filiere);
        fprintf(file, "supprimerLogiquement: %d\n", enregLog.suplogique);
        fprintf(file, "tailleDonneEnregistrement: %d\n", tailleenrgistrement);
        fclose(file);
        //ajoute d'un nouveau block selon le facteur de blockage et la position de current block allocation de block
        if(currentBlock<0)currentBlock=0;
        if (currentBlock >= fichierTOV->entete.nbrBlock || fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique >= facteurDeBlocage) {
            currentBlock = fichierTOV->entete.nbrBlock;
            fichierTOV->enregistrements[currentBlock].entete.id = fichierTOV->entete.nextId++;
            fichierTOV->enregistrements[currentBlock].entete.tailleDonnees = 0;
            fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique = 0;
            fichierTOV->entete.nbrBlock++;
        }
        insertionhachable(table,enregLog.id,fichierTOV->enregistrements[currentBlock].entete.id);//insertion de l'elment dans la table hachable
        
        //copie des element inserer dans le fichier Tov (stockage temporaire dans la structure TOV)
        sprintf(enregLogTov, "%d||%d||%d||%s||%s||%d||%s||",tailleenrgistrement,enregLog.suplogique,enregLog.id,enregLog.nom,chaine, enregLog.matricule, enregLog.filiere);//copie des information pour avoire le format souhaite
        memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, enregLogTov,strlen(enregLogTov));//l'alocation de memeoire pour le stockage da data + decalage au cas ou on a plusieur enregistrement logique
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += strlen(enregLogTov);//actualisation de la taille du block
        fichierTOV->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;//actualisation du nombre d'enregistrement logique
        
        // actualisaton du fichier  header (backend)
        file = fopen(cheminHeaderInfo, "w");
        if (file == NULL) {//verification de l'ouverture du fichier
            printf("Failed to open the file.\n");
            return;
        }
        //ecriture entete de fichier 
        fprintf(file, "FichierTOV Header:\n");
        fprintf(file, "nbEnregistrements: %d\n", fichierTOV->entete.nbEnregistrementslogique);
        fprintf(file, "nbrBlock: %d\n", fichierTOV->entete.nbrBlock);
        fprintf(file, "facteurdeblockage: %d\n", fichierTOV->entete.facteurdeblockage);
        fprintf(file, "nextId: %d\n", fichierTOV->entete.nextId);
        //ecriture de l'entete des block
        for(int i = 0; i < fichierTOV->entete.nbrBlock; i++) {
            fprintf(file, "\nEnregistrementPhysique %d Header:\n", i);
            fprintf(file, "id: %d\n", fichierTOV->enregistrements[i].entete.id);
            fprintf(file, "tailleDonnees: %d\n", fichierTOV->enregistrements[i].entete.tailleDonnees);
            fprintf(file, "nbrEnregistrementLogique: %d\n", fichierTOV->enregistrements[i].entete.nbrEnregistrementLogique);
        }

        fclose(file);
    }
    fichierTOV->enregistrements[currentBlock].dataBlock[fichierTOV->enregistrements[currentBlock].entete.tailleDonnees] = '\0';
    afficherDonneesFichierTOv(fichierTOV);//affichage des donne de TOV pour verifier le succes de l'insertion
     afficherTableHachage(table);
}

void suppressionStructuretemp(int id) {
    char cheminFichierLogique[] = "fichierLotest4.txt"; // Remplacez par le chemin vers votre fichier
    char cheminFichierTemp[] = "fichierLogiqueMouvement.txt"; // Remplacez par le chemin vers un fichier temporaire
    FILE* fichierLogique = fopen(cheminFichierLogique, "r");
    FILE* fichierTemp = fopen(cheminFichierTemp, "w");

    if (fichierLogique == NULL || fichierTemp == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    char ligne[256];
        while (fgets(ligne, sizeof(ligne), fichierLogique)) {
            int idLigne;
            sscanf(ligne, "id: %d", &idLigne);
            if (idLigne != id) {
                fputs(ligne, fichierTemp);
            }
        }
        // Réinitialiser les fichiers pour la prochaine suppression
        fclose(fichierLogique);
        fclose(fichierTemp);
        remove(cheminFichierLogique);
        rename(cheminFichierTemp, cheminFichierLogique);
}

void suppressionLogique(HashTable* table, FichierTOV* fichierTOV, int id) {
    // Search for the element in the hash table
    ElementTablHachabl* element = rechercherElement(table, id);
    if (element == NULL) {
        printf("Cet élément n'existe pas.\n");
        return;
    }

    // Find the corresponding block in FichierTOV
    int idBlock = element->idBlock;
    EnregistrementPhysique* block = &(fichierTOV->enregistrements[idBlock]);

    // Traverse the logical records in the block
    char* ptr = block->dataBlock;
    while (ptr < block->dataBlock + block->entete.tailleDonnees) {
        int tailleEnregLog, etatSuppression, idEnregLog;
        sscanf(ptr, "%d||%d||%d", &tailleEnregLog, &etatSuppression, &idEnregLog);

        // If the ID matches, check the etatSuppression
        if (idEnregLog == id) {
            if (etatSuppression == 0) {
                // Set etatSuppression to 1
                char* etatSuppressionPtr = strstr(ptr, "||") + 2;
                *etatSuppressionPtr = '1';

                // Call suppressionStructuretemp function to remove the record
                suppressionStructuretemp(id);
            } else {
                printf("Cet élément n'existe pas logiquement.\n");
            }
            break;
        }
        ptr += tailleEnregLog; // Move to the next logical record
    }
}





void supprimerPhysique(HashTable* table, FichierTOV* fichierTOV, int nbrSuppression) {
    int id;
    for (int i = 0; i < nbrSuppression; i++) {
        printf("Entrez l'ID de l'étudiant à supprimer: ");
        scanf("%d", &id);

        // Rechercher l'ID dans la table de hachage
        ElementTablHachabl* element = rechercherElement(table, id);
        if (element == NULL) {
            printf("Cet élément n'existe pas.\n");
            continue;
        }
        // Trouver le bloc correspondant dans FichierTOV
        int idBlock = element->idBlock;
        EnregistrementPhysique* block = &(fichierTOV->enregistrements[idBlock]);
        printf("%d",element->id);
        printf("%d\n",idBlock);
        // Parcourir les enregistrements logiques dans le bloc

        char* ptr = block->dataBlock;
        while (ptr < block->dataBlock + block->entete.tailleDonnees) {
            int tailleEnregLog, etatSuppression, idEnregLog;
            sscanf(ptr, "%d||%d||%d", &tailleEnregLog, &etatSuppression, &idEnregLog);
            printf("%d\n",idEnregLog);
            // Si l'ID correspond, vérifier l'état de suppression
            if (idEnregLog == id) {
                if (etatSuppression == 0) {
        etatSuppression = 1;
        sprintf(ptr, "%d||%d||%d", tailleEnregLog, etatSuppression, idEnregLog);
        suppressionStructuretemp(idEnregLog);                                  
    }
                // Décaler tous les enregistrements suivants vers la gauche
                memmove(ptr, ptr+tailleEnregLog, block->dataBlock + block->entete.tailleDonnees - (ptr+tailleEnregLog));
                block->entete.tailleDonnees -= tailleEnregLog; // Diminuer la taille des données
                block->entete.nbrEnregistrementLogique--; // Diminuer le nombre d'enregistrements logiques
                fichierTOV->entete.nbEnregistrementslogique--; // Diminuer le nombre total d'enregistrements logiques
            supprimerElement(table,idEnregLog);//suppression de l'element dans hachable    
                // Décaler tous les enregistrements dans les blocs suivants
                for (int j = idBlock + 1; j < fichierTOV->entete.nbrBlock; j++) {
                    EnregistrementPhysique* nextBlock = &(fichierTOV->enregistrements[j]);
                    int nextTailleEnregLog, nextEtatSuppression, nextIdEnregLog;
                    sscanf(nextBlock->dataBlock, "%d||%d||%d", &nextTailleEnregLog, &nextEtatSuppression, &nextIdEnregLog);
                    modifierIdBlock(table,nextIdEnregLog,nextBlock->entete.id-1);
                    // Déplacer le premier enregistrement du bloc suivant vers la fin du bloc actuel
                    memmove(block->dataBlock + block->entete.tailleDonnees, nextBlock->dataBlock, nextTailleEnregLog);
                    block->entete.tailleDonnees += nextTailleEnregLog;
                    block->entete.nbrEnregistrementLogique++;

                    // Décaler tous les enregistrements dans le bloc suivant
                    memmove(nextBlock->dataBlock, nextBlock->dataBlock + nextTailleEnregLog, nextBlock->entete.tailleDonnees - nextTailleEnregLog);
                    nextBlock->entete.tailleDonnees -= nextTailleEnregLog;
                    nextBlock->entete.nbrEnregistrementLogique--;

                    block = nextBlock;
                
                }
                break;
            }
            ptr += tailleEnregLog; // Passer à l'enregistrement logique suivant
        }
         if (block->entete.nbrEnregistrementLogique == 0) {
        fichierTOV->entete.nbrBlock--;
        fichierTOV->entete.nextId--; // Décrémenter le nombre de blocs et le nextid
    }
        afficherDonneesFichierTOv(fichierTOV);
        ecrireEnteteFichierTOV(fichierTOV,"headerInftest4.txt");
    }
}

int main(){

    fichierLogique fichier2;
    FichierTOV fichier;
    int facteurDeBlockage;
    HashTable* table = initialiserHachable();
    initialiserFichierTOV(&fichier,2,&fichier2);
    insertionLogique(table,&fichier2,&fichier,2,2,"fichierLotest4.txt","headerInftest4.txt");
    suppressionLogique(table,&fichier,3);
   supprimerPhysique(table,&fichier,2);
   chargementFinal(&fichier,"fichierLotest4.txt");
   
}






























