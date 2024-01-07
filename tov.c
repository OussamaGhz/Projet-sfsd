#include <stdio.h>
#include <gtk/gtk.h>

// import the tov.h
#include "tov.h"

// gtk configuration

// Signal handler for the "Add File" button
void on_button_add_clicked(GtkButton *button, gpointer user_data)
{
    GtkWindow *parent_window = GTK_WINDOW(user_data);

    // Create a new modal window
    GtkWidget *modal_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(modal_window), "Add Item");
    gtk_window_set_transient_for(GTK_WINDOW(modal_window), parent_window);
    gtk_window_set_modal(GTK_WINDOW(modal_window), TRUE);

    // Create a box to hold the labels and entries
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(modal_window), box);

    gtk_widget_set_margin_start(box, 40);
    gtk_widget_set_margin_end(box, 40);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);

    // Create and add the first name label and entry to the box
    GtkWidget *first_name_label = gtk_label_new("Enter first name:");
    GtkWidget *first_name_entry = gtk_entry_new();
    gtk_box_append(GTK_BOX(box), first_name_label);
    gtk_box_append(GTK_BOX(box), first_name_entry);

    // Create and add the second name label and entry to the box
    GtkWidget *second_name_label = gtk_label_new("Enter second name:");
    GtkWidget *second_name_entry = gtk_entry_new();
    gtk_box_append(GTK_BOX(box), second_name_label);
    gtk_box_append(GTK_BOX(box), second_name_entry);

    // Create and add the ID label and entry to the box
    GtkWidget *id_label = gtk_label_new("Enter ID (numbers only):");
    GtkWidget *id_entry = gtk_entry_new();
    gtk_entry_set_input_purpose(GTK_ENTRY(id_entry), GTK_INPUT_PURPOSE_DIGITS); // Sets input method for ID to accept only numbers
    gtk_box_append(GTK_BOX(box), id_label);
    gtk_box_append(GTK_BOX(box), id_entry);

    // Present the modal window
    gtk_window_present(GTK_WINDOW(modal_window));
}
// Activation callback
static void activate(GtkApplication *app, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *button_add;

    // Create a new builder from the Glade file
    builder = gtk_builder_new_from_file("design.glade~");

    // Get the main window and button from the builder
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    button_add = GTK_WIDGET(gtk_builder_get_object(builder, "button_add"));

    if (!window || !button_add)
    {
        g_printerr("Failed to fetch widgets from the Glade file\n");
        g_object_unref(builder);
        return;
    }

    // Set the application for the window and connect the signal for the "Add File" button
    gtk_window_set_application(GTK_WINDOW(window), app);
    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), window);

    // Show the window
    gtk_window_present(GTK_WINDOW(window)); // Present the main window

    // Release the builder
    g_object_unref(builder);
}

// -------------------------------------------------------------------------------------------------------------------------
// managing algorithms (pure c)
void initialiserFichierTOV(FichierTOV *fichier, int capaciteMax)
{
    fichier->entete.nbEnregistrements = 0;
    fichier->entete.capaciteMax = capaciteMax;
    fichier->enregistrements = (EnregistrementPhysique *)malloc(capaciteMax * sizeof(EnregistrementPhysique));

    if (fichier->enregistrements == NULL)
    {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < capaciteMax; i++)
    {
        fichier->enregistrements[i].donnees = NULL; // Initialiser les pointeurs à NULL
    }
}

void libererFichierTOV(FichierTOV *fichier)
{
    for (int i = 0; i < fichier->entete.nbEnregistrements; i++)
    {
        free(fichier->enregistrements[i].donnees);
    }
    free(fichier->enregistrements);
}

bool ajouterEnregistrement(FichierTOV *fichier, BufferTransmission *buffer)
{
    if (fichier->entete.nbEnregistrements >= fichier->entete.capaciteMax)
    {
        return false;
    }

    EnregistrementPhysique *enreg = &fichier->enregistrements[fichier->entete.nbEnregistrements];

    enreg->donnees = (char *)malloc(buffer->taille * sizeof(char));
    if (enreg->donnees == NULL)
    {
        return false;
    }

    strncpy(enreg->donnees, buffer->data, buffer->taille);
    enreg->entete.id = fichier->entete.nbEnregistrements;
    enreg->entete.tailleDonnees = buffer->taille;
    enreg->separateur = '|'; // Séparateur choisi

    fichier->entete.nbEnregistrements++;
    return true;
}

bool supprimerEnregistrement(FichierTOV *fichier, int id)
{
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

int main(int argc, char *argv[])
{
    GtkApplication *app;

    // Create a new application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the activation signal to the activation callback
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Start the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}