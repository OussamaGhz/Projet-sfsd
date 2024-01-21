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
        fichierLog->classe[i].id=-1;
        memset(fichierLog->classe[i].filiere, 0, sizeof(fichierLog->classe[i].filiere));
    }
    fichierLog->nbrEnregistrementLogique = 0;
printf(" fichier logique initialiser avec succes\n");

}
HashTable* initialiserHachable() {//initialisation de la table hachable
    HashTable* table = malloc(sizeof(HashTable));
    for (int i = 0; i < SIZE; i++) {
        table->tab[i] = NULL;
    }
    return table;
}
int positiontab(int cle) {
    return cle % SIZE;
}
 void insertionhachable(HashTable* table, int cle,int idBlock) {
    int index = positiontab(cle);
    ElementTablHachabl* newelement = malloc(sizeof(ElementTablHachabl));
    newelement->id = cle;
    newelement->idBlock = idBlock;
    newelement->next = table->tab[index];
    table->tab[index] = newelement;
}   
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
    fscanf(fichierLogiqueFile, "id: %d\n", &(enregLog.id));
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

//chargement des donnes header backend + donnes logique dans la structure TOV
void chargerDonneesFichierTOV(HashTable* table,FichierTOV* fichierTOV, const char* cheminHeaderInfo, const char* cheminFichierLogique) {
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
    insertionhachable(table,enregLog.id,fichierTOV->enregistrements[i].entete.id);
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
    sprintf(fichierTOV->enregistrements[i].dataBlock + strlen(fichierTOV->enregistrements[i].dataBlock), "%d||%s||%s||%d||%s||",enregLog.id, enregLog.nom,prenoms, enregLog.matricule, enregLog.filiere);
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

// verification de resussite de chargement
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
}//verification de reussite de l'insertion
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
 if(fichierTOV->entete.nbrBlock!=0){
     currentBlock=fichierTOV->entete.nbrBlock-1;
    }
    for(int i = 0; i < nbrInsertion; i++) {//verification de la capacite max
        if (fichierLog->nbrEnregistrementLogique >= MAX_ENREGISTREMENTS) {
            printf("le fichierLogique est remplie. on ne peut pas inserer plus d'enregistrement.\n");
            return;
        }
   // la saisie du nouvelle enregistrement a inserer
        enregistrementLogique enregLog;
        
        enregLog.id=fichierTOV->entete.nbEnregistrementslogique;                         //determination de l'id de chaque etudiant automatiquement
        printf("Enter nom: ");
        scanf("%s", enregLog.nom);
        
        int n;
        printf("entrer le nombre de vos prenom: ");
        scanf("%d",&n); //max 2 jusqu'a maintenant selon le format
        char chaine[40]="";//pour la concatenation (besois dans le stockage de data)

        for(int j = 0; j < n; j++) {
            enregLog.prenom[j] = malloc(20 * sizeof(char));
            printf("Enter prenom %d: ", j+1);
            scanf("%s", enregLog.prenom[j]);//insertiondes prenom
            strcat(chaine,enregLog.prenom[j]);//concatenation des prenoms dans chaine
        }

        printf("Enter matricule: ");
        scanf("%d", &(enregLog.matricule));

        printf("Enter filiere: ");
        scanf("%s", enregLog.filiere);
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
        insertionhachable(table,enregLog.id,fichierTOV->enregistrements[currentBlock].entete.id);

        //copie des element inserer dans le fichier Tov (stockage temporaire dans la structure TOV)
        char enregLogTov[256];//suport de stockage contenant le format de data souhaite
        sprintf(enregLogTov, "%d||%s||%s||%d||%s||",enregLog.id,enregLog.nom,chaine, enregLog.matricule, enregLog.filiere);//copie des information pour avoire le format souhaite
        memcpy(fichierTOV->enregistrements[currentBlock].dataBlock + fichierTOV->enregistrements[currentBlock].entete.tailleDonnees, enregLogTov,strlen(enregLogTov) + 1);//l'alocation de memeoire pour le stockage da data + decalage au cas ou on a plusieur enregistrement logique
        fichierTOV->enregistrements[currentBlock].entete.tailleDonnees += strlen(enregLogTov)+1;//actualisation de la taille du block
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
    afficherDonneesFichierTOv(fichierTOV);//affichage des donne de TOV pour verifier le succes de l'insertion
     afficherTableHachage(table);
}

   
bool recherchePhysique(FichierTOV* fichierTOV, int matriculeRecherche) {
    for(int i = 0; i < fichierTOV->entete.nbrBlock; i++) {
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
    HashTable* table = initialiserHachable();
    initialiserFichierTOV(&fichier,2,&fichier2);
    insertionLogique(table,&fichier2,&fichier,1,2,"fichierLotest4.txt","headerInftest4.txt");
    if(rechercherElement(table,1)==NULL){
        printf("l'element n'existe pas");
    }else{
     printf("l'element a etais trouver");
    }



}































