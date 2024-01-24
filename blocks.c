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
    int block_height = 30; 
    int y = 20;            

    while (token != NULL)
    {
        // Draw a rectangular block
        cairo_rectangle(cr, 20, y, 200, block_height);
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_fill(cr);

        // Draw the element text inside the block
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_move_to(cr, 30, y + 20);            
        cairo_show_text(cr, token);               

        // Move to the next block position
        y += block_height + 10; // Add spacing between blocks
        token = strtok(NULL, "\n");
    }

    return FALSE;
}