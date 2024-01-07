#include <gtk/gtk.h>
#include "tov.h"

// Forward declarations of signal handlers
void on_button_add_clicked(GtkButton *button, gpointer user_data);
void on_button_delete_clicked(GtkButton *button, gpointer user_data);
void on_button_show_content_clicked(GtkButton *button, gpointer user_data);
void on_button_modify_content_clicked(GtkButton *button, gpointer user_data);

// Activation callback
static void activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *button_add, *button_delete, *button_show_content, *button_modify_content;

    // Initialize your TOV file (replace with actual initialization)
    FichierTOV *fichier = g_malloc(sizeof(FichierTOV));
    initialiserFichierTOV(fichier, MAX_ENREGISTREMENTS);

    // Create a new builder from the Glade file
    builder = gtk_builder_new_from_file("design.glade");

    // Get the main window and buttons from the builder
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    button_add = GTK_WIDGET(gtk_builder_get_object(builder, "button_add"));
    button_delete = GTK_WIDGET(gtk_builder_get_object(builder, "button_delete"));
    button_show_content = GTK_WIDGET(gtk_builder_get_object(builder, "button_show_content"));
    button_modify_content = GTK_WIDGET(gtk_builder_get_object(builder, "button_modify_content"));

    if (!window || !button_add || !button_delete || !button_show_content || !button_modify_content)
    {
        g_printerr("Failed to fetch widgets from the Glade file\n");
        g_object_unref(builder);
        return;
    }

    // Set the application for the window and connect signals
    gtk_window_set_application(GTK_WINDOW(window), app);
    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), fichier);
    g_signal_connect(button_delete, "clicked", G_CALLBACK(on_button_delete_clicked), fichier);
    g_signal_connect(button_show_content, "clicked", G_CALLBACK(on_button_show_content_clicked), fichier);
    g_signal_connect(button_modify_content, "clicked", G_CALLBACK(on_button_modify_content_clicked), fichier);

    // Show the window
    gtk_window_present(GTK_WINDOW(window));

    // Release the builder
    g_object_unref(builder);
}

// Signal handler for the "Add" button
void on_button_add_clicked(GtkButton *button, gpointer user_data)
{
    // Implement add functionality
    // For example, open a dialog to get new record details, then add it to the TOV file
    g_print("Add Button Clicked\n");
}

// Signal handler for the "Delete" button
void on_button_delete_clicked(GtkButton *button, gpointer user_data)
{
    // Implement delete functionality
    // For example, prompt for an ID and then delete the corresponding record
    g_print("Delete Button Clicked\n");
}

// Signal handler for the "Show Content" button
void on_button_show_content_clicked(GtkButton *button, gpointer user_data)
{
    // Implement show content functionality
    // For example, iterate through the TOV file and display its contents
    g_print("Show Content Button Clicked\n");
}

// Signal handler for the "Modify Content" button
void on_button_modify_content_clicked(GtkButton *button, gpointer user_data)
{
    // Implement modify content functionality
    // For example, open a dialog to get new content for an existing record
    g_print("Modify Content Button Clicked\n");
}

// ---------------------------------------------------------------------------------------------
// algorithmes
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax)
{
    if (fichier == NULL)
        return;

    fichier->entete.nbEnregistrements = 0;
    fichier->entete.capaciteMax = capaciteMax;
    fichier->enregistrements = malloc(capaciteMax * sizeof(EnregistrementPhysique));
    for (int i = 0; i < capaciteMax; i++)
    {
        fichier->enregistrements[i].donnees = NULL;
    }
}

void libererFichierTOV(FichierTOV *fichier)
{
    if (fichier == NULL)
        return;

    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        free(fichier->enregistrements[i].donnees);
    }
    free(fichier->enregistrements);
}

bool ajouterEnregistrement(FichierTOV *fichier, BufferTransmission *buffer)
{
    if (fichier == NULL || buffer == NULL || fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax)
    {
        return false;
    }

    EnregistrementPhysique *enreg = &fichier->enregistrements[fichier->entete.nbEnregistrements];
    enreg->donnees = malloc((buffer->taille + 1) * sizeof(char));
    if (enreg->donnees == NULL)
    {
        return false;
    }
    strncpy(enreg->donnees, buffer->data, buffer->taille);
    enreg->donnees[buffer->taille] = '\0';
    enreg->entete.id = fichier->entete.nbEnregistrements;
    enreg->entete.tailleDonnees = buffer->taille;
    enreg->separateur = '|';

    fichier->entete.nbEnregistrements++;
    return true;
}

bool supprimerEnregistrement(FichierTOV *fichier, int id)
{
    if (fichier == NULL)
        return false;

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

EnregistrementPhysique *rechercherEnregistrement(FichierTOV *fichier, int id)
{
    if (fichier == NULL)
        return NULL;

    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        if (fichier->enregistrements[i].entete.id == id)
        {
            return &fichier->enregistrements[i];
        }
    }
    return NULL;
}

void afficherFichierTOV(const FichierTOV *fichier)
{
    if (fichier == NULL)
        return;

    printf("Fichier TOV contient %d enregistrements:\n", fichier->entete.nbEnregistrements);
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        printf("Enregistrement %d: %s\n", fichier->enregistrements[i].entete.id, fichier->enregistrements[i].donnees);
    }
}

void remplirBuffer(BufferTransmission *buffer, const char *data)
{
    if (buffer == NULL || data == NULL)
        return;

    size_t dataLen = strlen(data);
    if (dataLen >= TAILLE_BUFFER)
    {
        dataLen = TAILLE_BUFFER - 1;
    }
    strncpy(buffer->data, data, dataLen);
    buffer->data[dataLen] = '\0';
    buffer->taille = dataLen;
}

void viderBuffer(BufferTransmission *buffer)
{
    if (buffer == NULL)
        return;

    memset(buffer->data, 0, TAILLE_BUFFER);
    buffer->taille = 0;
}

unsigned long CalculerTailleEnregistrement(const EnregistrementPhysique *enregistrement)
{
    if (enregistrement == NULL)
        return 0;

    return sizeof(EnteteEnregistrement) + sizeof(char) + strlen(enregistrement->donnees);
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    // Create a new application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the activation signal to the activation callback
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Start the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
