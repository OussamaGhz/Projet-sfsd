#include <gtk/gtk.h>
#include <glib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "tov.h"

// Forward declarations of signal handlers
void on_button_create_clicked(GtkButton *button, gpointer user_data);
void on_button_add_clicked(GtkButton *button, gpointer user_data);
void on_button_delete_clicked(GtkButton *button, gpointer user_data);
void on_button_show_content_clicked(GtkButton *button, gpointer user_data);
void on_button_quit_clicked(GtkButton *button, gpointer user_data);
void on_button_confirm_clicked(GtkButton *button, gpointer user_data);
void on_button_confirm_create_clicked(GtkButton *button, gpointer user_data);
void on_button_confirm_delete_clicked(GtkButton *button, gpointer user_data);

void initialiserHashTable(HashTable *hashTable, int taille);

gboolean confirm_create_clicked = FALSE;
GtkApplication *global_app = NULL;

FichierTOV fichier;
HashTable hashTable;
BufferTransmission buffer;
EnregistrementPhysique newEnregistrement;

// Activation callback
static void activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *modal_window;
    GtkWidget *modal_window_inf;
    GtkWidget *button_confirm_delete;
    GtkWidget *button_add, *button_create, *button_delete, *button_show_content, *button_quit, *button_confirm, *button_confirm_create;
    GtkWidget *label_content;

    // Initialize your TOV file (replace with actual initialization)
    FichierTOV *fichier = g_malloc(sizeof(FichierTOV));

    // Create a new builder from the Glade file
    builder = gtk_builder_new_from_file("design.glade");

    // Get the main window and buttons from the builder
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    button_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_create"));
    button_add = GTK_WIDGET(gtk_builder_get_object(builder, "button_add"));
    button_delete = GTK_WIDGET(gtk_builder_get_object(builder, "button_delete"));
    button_show_content = GTK_WIDGET(gtk_builder_get_object(builder, "button_show_content"));
    button_quit = GTK_WIDGET(gtk_builder_get_object(builder, "button_quit"));
    button_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm"));
    button_confirm_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_create"));
    button_confirm_delete = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_delete"));
    label_content = GTK_WIDGET(gtk_builder_get_object(builder, "label_content"));

    if (!window || !button_create || !button_add || !button_delete || !button_show_content || !button_quit || !button_confirm || !button_confirm_create || !button_confirm_delete)
    {
        g_printerr("Failed to fetch widgets from the Glade file\n");
        g_object_unref(builder);
        return;
    }

    // Set the application for the window and connect signals
    gtk_window_set_application(GTK_WINDOW(window), app);
    

    // Connect signals
    g_signal_connect(button_create, "clicked", G_CALLBACK(on_button_create_clicked), builder);
    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), builder);
    g_signal_connect(button_delete, "clicked", G_CALLBACK(on_button_delete_clicked), builder);
    g_signal_connect(button_show_content, "clicked", G_CALLBACK(on_button_show_content_clicked), builder);
    g_signal_connect(button_quit, "clicked", G_CALLBACK(on_button_quit_clicked), builder);

    button_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm"));
    button_confirm_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_create"));

    modal_window = GTK_WIDGET(gtk_builder_get_object(builder, "add_item_modal"));
    modal_window_inf = GTK_WIDGET(gtk_builder_get_object(builder, "information_modal"));

    if (!modal_window || !modal_window_inf)
    {
        g_printerr("Failed to fetch modal window from the Glade file\n");
        g_object_unref(builder);
        return;
    }

    // Set the modal window for the "Confirm" button
    g_signal_connect(button_confirm, "clicked", G_CALLBACK(on_button_confirm_clicked), builder);
    g_signal_connect(button_confirm_create, "clicked", G_CALLBACK(on_button_confirm_create_clicked), builder);
    g_signal_connect(button_confirm_delete, "clicked", G_CALLBACK(on_button_confirm_delete_clicked), builder);

    // Show the main window
    gtk_window_present(GTK_WINDOW(window));

    // Release the builder
    g_object_unref(builder);
}

// Signal handler for the "Add" button
void on_button_create_clicked(GtkButton *button, gpointer user_data)
{
    printf("Create button was clicked\n");

    // Declare the modal and confirm button locally
    GtkBuilder *builder = gtk_builder_new_from_file("design.glade");
    GtkWidget *modal_window_inf = GTK_WIDGET(gtk_builder_get_object(builder, "information_modal"));
    GtkWidget *button_confirm_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_create"));

    // Get the top-level parent window from the button widget using gtk_widget_get_ancestor
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW));

    // Corrected variable name: use modal_window_inf instead of information_modal
    gtk_window_set_transient_for(GTK_WINDOW(modal_window_inf), parent_window);

    // Set the modal window for the "Confirm" button
    g_signal_connect(button_confirm_create, "clicked", G_CALLBACK(on_button_confirm_create_clicked), builder);

    

    // Present the modal window using gtk_window_present
    gtk_window_present(GTK_WINDOW(modal_window_inf));

    // Release the builder
    // g_object_unref(builder);
}

void on_button_add_clicked(GtkButton *button, gpointer user_data)
{
    printf("Add button was clicked\n");

    // Declare the modal and confirm button locally
    GtkBuilder *builder = gtk_builder_new_from_file("design.glade");
    GtkWidget *modal_window = GTK_WIDGET(gtk_builder_get_object(builder, "add_item_modal"));
    GtkWidget *confirm_button = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm"));

    // Set parent window for the modal
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW));
    gtk_window_set_transient_for(GTK_WINDOW(modal_window), parent_window);

    // Set the modal window for the "Confirm" button
    g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_button_confirm_clicked), builder);

    // Present the modal window using gtk_window_present
    gtk_window_present(GTK_WINDOW(modal_window));
}
void on_button_confirm_create_clicked(GtkButton *button, gpointer user_data)
{
    // Assuming user_data contains the main window's builder
    GtkBuilder *main_builder = GTK_BUILDER(user_data);

    GtkWidget *modal_window_inf = GTK_WIDGET(gtk_builder_get_object(main_builder, "information_modal"));
    GtkWidget *length_entry = GTK_WIDGET(gtk_builder_get_object(main_builder, "length_entry"));

    GtkWidget *length_error_label = GTK_WIDGET(gtk_builder_get_object(main_builder, "label_length_error"));

    const char *length_text = gtk_editable_get_text(GTK_EDITABLE(length_entry));

    // Form validation
    if (strlen(length_text) == 0)
    {
        gtk_widget_set_visible(length_error_label, TRUE);
        return;
    }
    else
    {
        gtk_widget_set_visible(length_error_label, FALSE);

        // Additional validation: Check if the input is a number
        for (int i = 0; length_text[i] != '\0'; i++)
        {
            if (!isdigit(length_text[i]))
            {
                // If not a number, show error label and return
                gtk_widget_set_visible(length_error_label, TRUE);
                return;
            }
        }
    }

    // If it passes both checks, hide the error labels
    gtk_widget_set_visible(length_error_label, FALSE);

    // initilaise the folder after confirming
    int capaciteFichier = atoi(length_text);
    initialiserFichierTOV(&fichier, capaciteFichier);

    initialiserHashTable(&hashTable, capaciteFichier);

    // Hide the modal window after confirming
    gtk_widget_set_visible(modal_window_inf, FALSE);
}

void on_button_confirm_clicked(GtkButton *button, gpointer user_data)
{
    GtkBuilder *builder = GTK_BUILDER(user_data);

    GtkWidget *modal_window = GTK_WIDGET(gtk_builder_get_object(builder, "add_item_modal"));
    GtkWidget *first_name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "first_name_entry"));
    GtkWidget *second_name_entry = GTK_WIDGET(gtk_builder_get_object(builder, "second_name_entry"));
    GtkWidget *id_entry = GTK_WIDGET(gtk_builder_get_object(builder, "id_entry"));

    GtkWidget *first_name_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "label_first_name_error"));
    GtkWidget *second_name_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "label_second_name_error"));
    GtkWidget *id_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "label_id_error"));

    const char *first_name = gtk_editable_get_text(GTK_EDITABLE(first_name_entry));
    const char *second_name = gtk_editable_get_text(GTK_EDITABLE(second_name_entry));
    const char *id_text = gtk_editable_get_text(GTK_EDITABLE(id_entry));

    // Form validation
    if (strlen(first_name) == 0)
    {
        gtk_widget_set_visible(first_name_error_label, TRUE);
        return;
    }
    else
    {
        gtk_widget_set_visible(first_name_error_label, FALSE);
    }

    if (strlen(second_name) == 0)
    {
        gtk_widget_set_visible(second_name_error_label, TRUE);
        return;
    }
    else
    {
        gtk_widget_set_visible(second_name_error_label, FALSE);
    }

    if (strlen(id_text) == 0)
    {
        gtk_widget_set_visible(id_error_label, TRUE);
        return;
    }
    else
    {
        gtk_widget_set_visible(id_error_label, FALSE);

        // Additional validation: Check if the input is a number
        for (int i = 0; id_text[i] != '\0'; i++)
        {
            if (!isdigit(id_text[i]))
            {
                // If not a number, show error label and return
                gtk_widget_set_visible(id_error_label, TRUE);
                return;
            }
        }

        // If it passes both checks, hide the error label
        gtk_widget_set_visible(id_error_label, FALSE);
    }

    strcpy(newEnregistrement.data1, first_name);
    strcpy(newEnregistrement.data2, second_name);
    strcpy(newEnregistrement.data3, id_text);

    if (ajouterEnregistrement(&fichier, &hashTable, &newEnregistrement))
    {
        printf("Enregistrement ajoute avec succes\n");
    }
    else
    {
        printf("Erreur lors de l'ajout de l'enregistrement\n");
    }

    // Hide the modal window after confirming
    gtk_widget_set_visible(modal_window, FALSE);
}

void on_button_quit_clicked(GtkButton *button, gpointer user_data)
{
    printf("Quit button was clicked\n");

    // Free any allocated resources here
    libererFichierTOV(&fichier);

    // Use the global application reference to quit the application
    if (global_app != NULL) {
        g_application_quit(G_APPLICATION(global_app));
    }
}


void on_button_show_content_clicked(GtkButton *button, gpointer user_data)
{
    GtkBuilder *main_builder = GTK_BUILDER(user_data);
    GtkWidget *label_content = GTK_WIDGET(gtk_builder_get_object(main_builder, "label_content"));

    const char *nomFichier = "monFichierTOV.tov";
    FILE *fichierPhysique = fopen(nomFichier, "r");

    if (fichierPhysique == NULL)
    {
        g_printerr("Failed to open file for reading\n");
        return;
    }

    fseek(fichierPhysique, 0, SEEK_END);
    long file_size = ftell(fichierPhysique);
    fseek(fichierPhysique, 0, SEEK_SET);

    char *file_content = malloc(file_size + 1);

    if (file_content == NULL)
    {
        g_printerr("Memory allocation failed\n");
        fclose(fichierPhysique);
        return;
    }

    if (fread(file_content, 1, file_size, fichierPhysique) != file_size)
    {
        g_printerr("Failed to read file\n");
        fclose(fichierPhysique);
        free(file_content);
        return;
    }

    file_content[file_size] = '\0';

    gtk_label_set_text(GTK_LABEL(label_content), file_content);

    fclose(fichierPhysique);
    free(file_content);
}

void on_button_delete_clicked(GtkButton *button, gpointer user_data)
{
    printf("Delete button was clicked\n");

    // Declare the modal and confirm button locally
    GtkBuilder *builder = gtk_builder_new_from_file("design.glade");
    GtkWidget *modal_window_delete = GTK_WIDGET(gtk_builder_get_object(builder, "delete_modal"));
    GtkWidget *confirm_button_delete = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_delete"));

    // Set parent window for the modal
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW));
    gtk_window_set_transient_for(GTK_WINDOW(modal_window_delete), parent_window);

    // Set the modal window for the "Confirm" button
    g_signal_connect(confirm_button_delete, "clicked", G_CALLBACK(on_button_confirm_delete_clicked), builder);

    // Present the modal window using gtk_window_present
    gtk_window_present(GTK_WINDOW(modal_window_delete));

    // Release the builder
    // g_object_unref(builder);
}

void on_button_confirm_delete_clicked(GtkButton *button, gpointer user_data)
{
    // Assuming user_data contains the main window's builder
    GtkBuilder *main_builder = GTK_BUILDER(user_data);

    GtkWidget *modal_window_delete = GTK_WIDGET(gtk_builder_get_object(main_builder, "delete_modal"));
    GtkWidget *id_entry_delete = GTK_WIDGET(gtk_builder_get_object(main_builder, "id_delete_entry"));
    GtkWidget *id_error_label_delete = GTK_WIDGET(gtk_builder_get_object(main_builder, "label_id_delete_error"));

    const char *id_text_delete = gtk_editable_get_text(GTK_EDITABLE(id_entry_delete));

    // Additional validation: Check if the input is a non-empty numeric string
    for (int i = 0; id_text_delete[i] != '\0'; i++)
    {
        if (!isdigit(id_text_delete[i]))
        {
            // If not a number, show error label and return
            gtk_widget_set_visible(id_error_label_delete, TRUE);
            return;
        }
    }

    // Form validation
    if (strlen(id_text_delete) == 0)
    {
        gtk_widget_set_visible(id_error_label_delete, TRUE);
        return;
    }
    else
    {
        gtk_widget_set_visible(id_error_label_delete, FALSE);
    }

    int id = atoi(id_text_delete);
    supprimerEnregistrement(&fichier, &hashTable, id);

    // If it passes both checks, hide the error labels
    gtk_widget_set_visible(id_error_label_delete, FALSE);

    // Perform delete operation here

    // Hide the modal window after confirming
    gtk_widget_set_visible(modal_window_delete, FALSE);
}

// ---------------------------------------------------------------------------------------------
// algorithmes

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

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;
    printf("hello world");

    // Create a new application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    global_app = app;  // Set the global variable

    // Connect the activation signal to the activation callback
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Start the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}

