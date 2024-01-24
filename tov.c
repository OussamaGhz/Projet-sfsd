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

GtkApplication *global_app = NULL;

GtkWidget *button_add;
GtkWidget *button_delete;
GtkWidget *button_show_content;
GtkWidget *button_create;

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
    // Set the application for the window and connect signals
    gtk_window_set_application(GTK_WINDOW(window), app);

    button_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_create"));
    button_add = GTK_WIDGET(gtk_builder_get_object(builder, "button_add"));
    button_delete = GTK_WIDGET(gtk_builder_get_object(builder, "button_delete"));
    button_show_content = GTK_WIDGET(gtk_builder_get_object(builder, "button_show_content"));
    button_quit = GTK_WIDGET(gtk_builder_get_object(builder, "button_quit"));
    button_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm"));
    button_confirm_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_create"));
    button_confirm_delete = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_delete"));
    label_content = GTK_WIDGET(gtk_builder_get_object(builder, "label_content"));
    button_confirm = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm"));
    button_confirm_create = GTK_WIDGET(gtk_builder_get_object(builder, "button_confirm_create"));

    if (!button_create || !button_add || !button_delete || !button_show_content || !button_quit || !button_confirm || !button_confirm_create || !button_confirm_delete)
    {
        g_printerr("Failed to fetch widgets from the Glade file\n");
        g_object_unref(builder);
        return;
    }

    // Connect signals
    g_signal_connect(button_create, "clicked", G_CALLBACK(on_button_create_clicked), builder);
    g_signal_connect(button_add, "clicked", G_CALLBACK(on_button_add_clicked), builder);
    g_signal_connect(button_delete, "clicked", G_CALLBACK(on_button_delete_clicked), builder);
    g_signal_connect(button_show_content, "clicked", G_CALLBACK(on_button_show_content_clicked), builder);
    g_signal_connect(button_quit, "clicked", G_CALLBACK(on_button_quit_clicked), builder);
    g_signal_connect(button_confirm, "clicked", G_CALLBACK(on_button_confirm_clicked), builder);
    g_signal_connect(button_confirm_create, "clicked", G_CALLBACK(on_button_confirm_create_clicked), builder);
    g_signal_connect(button_confirm_delete, "clicked", G_CALLBACK(on_button_confirm_delete_clicked), builder);

    modal_window = GTK_WIDGET(gtk_builder_get_object(builder, "add_item_modal"));
    modal_window_inf = GTK_WIDGET(gtk_builder_get_object(builder, "information_modal"));

    if (!modal_window || !modal_window_inf)
    {
        g_printerr("Failed to fetch modal window from the Glade file\n");
        g_object_unref(builder);
        return;
    }

    // Show the main window
    gtk_window_present(GTK_WINDOW(window));
}

// Signal handler for the "Add" button
void on_button_create_clicked(GtkButton *button, gpointer user_data)
{
    printf("Create button was clicked\n");

    // Declare the modal and confirm button locally
    GtkBuilder *builder = GTK_BUILDER(user_data);
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
}

void on_button_add_clicked(GtkButton *button, gpointer user_data)
{
    printf("Add button was clicked\n");

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

    // Get the buttons where you want to set visibility
    button_add = GTK_WIDGET(gtk_builder_get_object(main_builder, "button_add"));
    button_delete = GTK_WIDGET(gtk_builder_get_object(main_builder, "button_delete"));
    button_show_content = GTK_WIDGET(gtk_builder_get_object(main_builder, "button_show_content"));
    button_show_content = GTK_WIDGET(gtk_builder_get_object(main_builder, "button_show_content"));
    button_create = GTK_WIDGET(gtk_builder_get_object(main_builder, "button_create"));

    // Set the visibility of these buttons to TRUE
    gtk_widget_set_visible(button_add, TRUE);
    gtk_widget_set_visible(button_delete, TRUE);
    gtk_widget_set_visible(button_show_content, TRUE);
    gtk_widget_set_visible(button_create, FALSE);

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
    if (global_app != NULL)
    {
        g_application_quit(G_APPLICATION(global_app));
    }
}

void on_button_show_content_clicked(GtkButton *button, gpointer user_data)
{
    // Assuming user_data contains the main window's builder
    GtkBuilder *main_builder = GTK_BUILDER(user_data);

    // // Get the label widget where you want to display the content
    GtkWidget *label_content = GTK_WIDGET(gtk_builder_get_object(main_builder, "label_content"));

    // Open the TOV file for reading
    const char *nomFichier = "monFichierTOV.tov";
    FILE *fichierPhysique = fopen(nomFichier, "r");

    // Check if the file was opened successfully
    if (fichierPhysique == NULL)
    {
        printf("Error opening the TOV file for reading.\n");
        return;
    }

    // Read and display the content line by line
    char ligne[1024];
    gchar *content_text = g_strdup(""); // Initialize an empty string

    while (fgets(ligne, sizeof(ligne), fichierPhysique) != NULL)
    {
        // Append the current line to the content text
        content_text = g_strconcat(content_text, ligne, NULL);
        printf(ligne);
    }

    // Close the file
    fclose(fichierPhysique);

    // Set the content text to the label
    gtk_label_set_text(GTK_LABEL(label_content), content_text);

    // Free the allocated memory for content_text
    g_free(content_text);
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

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;
    printf("hello world\n");

    // Create a new application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    global_app = app; // Set the global variable

    // Connect the activation signal to the activation callback
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Start the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}
