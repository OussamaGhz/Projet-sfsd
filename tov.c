#include <stdio.h>
#include "tov.h"
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



   //fonction d'insertion logique et phisique dans un fichier
   void insertionLogique(fichierLogique* fichierLog, FichierTOV *fichier,int nbrinsertion,int facteurDeBlockage) {
    
    int currentBlock = 0;//vas permmmetre de resoudre les probleme de logique
    for(int i = 0; i < nbrinsertion; i++) {
        if (fichierLog->nbrEnregistrementLogique >= MAX_ENREGISTREMENTS) {
            printf("le fichierLogique est remplie. on ne peut pas inserer plus d'enregistrement.\n");
            return;
        }
      //la saisie de l'enregistrement logique
        enregistrementLogique enregLog;

        printf("Enter nom: ");
        scanf("%s", enregLog.nom);
        int n;
        printf("entrer le nombre de vos prenom: ");//taille variable
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
        // la copie dans le fichier logique
        fichierLog->classe[fichierLog->nbrEnregistrementLogique] = enregLog;
        fichierLog->nbrEnregistrementLogique++;
        // Creation d'un fichier .txt pour stocker les donnes

    FILE* file = fopen("fichierLogique.txt", "a");
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
       // le stockage sur le fichier phisique
        // cas ou on est au premier block ou cas où on vas à un nouveau block
        if (currentBlock >= fichier->entete.nbEnregistrements || fichier->enregistrements[currentBlock].entete.nbrEnregistrementLogique >= facteurDeBlockage) {
            currentBlock = fichier->entete.nbEnregistrements;
            fichier->enregistrements[currentBlock].entete.id = fichier->entete.nextId++;
            fichier->enregistrements[currentBlock].entete.tailleDonnees = 0;
            fichier->enregistrements[currentBlock].entete.nbrEnregistrementLogique = 0;
            fichier->entete.nbEnregistrements++;
            fichier->entete.nbrBlock++;
        }

        //finaliser le stockage
        memcpy(fichier->enregistrements[currentBlock].dataBlock + fichier->enregistrements[currentBlock].entete.tailleDonnees, &enregLog, sizeof(enregistrementLogique));
        fichier->enregistrements[currentBlock].entete.tailleDonnees += sizeof(enregistrementLogique);
        fichier->enregistrements[currentBlock].entete.nbrEnregistrementLogique++;
    
}









}






    



 









int main(){
    fichierLogique fichier2;
    FichierTOV fichier;
    int facteurDeBlockage;
    initialiserFichierTOV(&fichier,2,&fichier2);
insertionLogique(&fichier2,&fichier,2,2);


}
