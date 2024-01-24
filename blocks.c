#include <gtk/gtk.h>

// Global variables for the drawing area and content
GtkWidget *drawing_area;
char *file_content = NULL;

// Function to draw the blocks
static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    if (file_content == NULL)
        return FALSE;

    // Split the content into blocks
    char *token = strtok(file_content, "\n");
    int block_height = 30; // Height of each block
    int y = 20;            // Initial y position

    while (token != NULL)
    {
        // Draw a rectangular block
        cairo_rectangle(cr, 20, y, 200, block_height);
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Set color (black)
        cairo_fill(cr);

        // Draw the element text inside the block
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Set color (white)
        cairo_move_to(cr, 30, y + 20);            // Position for text
        cairo_show_text(cr, token);               // Display the element

        // Move to the next block position
        y += block_height + 10; // Add spacing between blocks
        token = strtok(NULL, "\n");
    }

    return FALSE;
}