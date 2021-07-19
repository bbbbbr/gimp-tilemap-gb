#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "logging.h"

#include "export-dialog.h"

#define ARRAY_LEN(x)  (int)(sizeof(x) / sizeof((x)[0]))

extern const char BINARY_NAME[];

// These should match the enumeration of tile_process_modes
const char STR_OPT_GB_MODE_DMG[] = "DMG 4 Color";
const char STR_OPT_GB_MODE_CGB[] = "CGB 32 Color";

const char STR_OPT_TILE_SIZE_8x8[]   =  "8 x 8";
const char STR_OPT_TILE_SIZE_8x16[]  =  "8 x 16";
const char STR_OPT_TILE_SIZE_16x16[] = "16 x 16";
const char STR_OPT_TILE_SIZE_32x32[] = "32 x 32";

const char * tile_process_mode_strs[] = {STR_OPT_GB_MODE_DMG, STR_OPT_GB_MODE_CGB};

const char * tile_process_tilesize_strs[] = {STR_OPT_TILE_SIZE_8x8, STR_OPT_TILE_SIZE_8x16, STR_OPT_TILE_SIZE_16x16, STR_OPT_TILE_SIZE_32x32};

GtkWidget * combo_gb_mode;
GtkWidget * combo_tilesize;
GtkWidget * check_dedupe_tiles;
GtkWidget * check_dedupe_on_flip;
GtkWidget * check_dedupe_on_palette;
GtkWidget * check_ignore_palette_errors;
GtkWidget * check_repair_palette_errors;
GtkWidget * check_use_remap_pal_file;

static tile_process_options * p_plugin_options;

static GtkWidget * dialog;

static void on_response(GtkDialog *, gint, gpointer);

static void combo_set_active_entry_by_string(GtkWidget *combo, gchar * string_to_match);

static void on_settings_gb_mode_combo_changed(GtkComboBox *combo, gpointer callback_data);
static void on_settings_tilesize_combo_changed(GtkComboBox *combo, gpointer callback_data);
static void on_settings_checkbutton_changed(GtkToggleButton * p_togglebutton, gpointer callback_data);
static void on_settings_remap_pal_file_changed(GtkToggleButton * p_togglebutton, gpointer callback_data);
static void update_enabled_ui_controls(void);

static void on_response(GtkDialog * this_dialog, gint response_id, gpointer user_data) {

    // Quit the loop
    gtk_main_quit(); // TODO: delete?


    // Disable other dedupe options if the main tile dedupe isn't enabled
    if (!p_plugin_options->tile_dedupe_enabled) {
        p_plugin_options->tile_dedupe_flips = false;
        p_plugin_options->tile_dedupe_palettes = false;
    }

    // Capture and map the response ID to a return code
    if (response_id == GTK_RESPONSE_OK) {

        if (p_plugin_options->gb_mode == MODE_ERROR_TOO_MANY_COLORS)
            *((int *)user_data) = false;
        else
            *((int *)user_data) = true; // pointer to response var in dialog
        // OPTIONAL: *((int *)user_data) = response_id;
    }
}



int export_dialog(tile_process_options * p_src_plugin_options, const char * plugin_procedure_name) {

    int response = false;

    GtkWidget * vbox;
    GtkWidget * label;
    GtkWidget * label_dedupe;
    int         c;

    // Copy options into local glboal
    p_plugin_options = p_src_plugin_options;

        // === Create the Export Dialog ===

        // If Exporting use the export dialog convenience function
        dialog = gimp_export_dialog_new("GBTD/GBMB",
                                        BINARY_NAME,
                                        plugin_procedure_name);

        // Create the VBox
        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
        gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                           vbox, true, true, 2);
        gtk_widget_show(vbox);


        // Create the label
        label = gtk_label_new("Game Boy Tile Set / Tile Map Export options.");
        gtk_box_pack_start(GTK_BOX(vbox), label, false, false, 2);
        gtk_widget_show(label);


        // Create a combo/list box for selecting the mode
        // Then add the mode select entries
        combo_gb_mode = gtk_combo_box_text_new();
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_gb_mode), STR_OPT_GB_MODE_DMG);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_gb_mode), STR_OPT_GB_MODE_CGB);
        // Select default value
        // gtk_combo_box_set_active(GTK_COMBO_BOX(combo_gb_mode), p_plugin_options->gb_mode);
        combo_set_active_entry_by_string(combo_gb_mode, (gchar *) tile_process_mode_strs[p_plugin_options->gb_mode] );
        // Add it to the box for display and show it
        gtk_box_pack_start(GTK_BOX(vbox), combo_gb_mode, false, false, 2);
        gtk_widget_show(combo_gb_mode);


        // TODO: For default tile size: GBR: try to use image width (8,16,32), GBM/C: use 8x8
        int opt_tilesize = 0; // 8x8 is default for now

        if ((p_plugin_options->tile_width == 8) && (p_plugin_options->tile_height == 8))
            opt_tilesize = 0;
        else if ((p_plugin_options->tile_width == 8) && (p_plugin_options->tile_height == 16))
            opt_tilesize = 1;
        else if ((p_plugin_options->tile_width == 16) && (p_plugin_options->tile_height == 16))
            opt_tilesize = 2;
        else if ((p_plugin_options->tile_width == 32) && (p_plugin_options->tile_height == 32))
            opt_tilesize = 3;

        // Create a combo/list box for selecting tile size
        // Then add the tile size select entries
        // Then  it to the box for display and show it
        combo_tilesize = gtk_combo_box_text_new();
        for (c = 0; c < sizeof(tile_process_tilesize_strs) / sizeof(tile_process_tilesize_strs[0]); c++)
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tilesize), tile_process_tilesize_strs[c]);
        combo_set_active_entry_by_string(combo_tilesize, (gchar *) tile_process_tilesize_strs[opt_tilesize]);
        gtk_box_pack_start(GTK_BOX(vbox), combo_tilesize, false, false, 2);
        gtk_widget_show(combo_tilesize);


        // == Map export options ==


        check_ignore_palette_errors = gtk_check_button_new_with_label("Ignore CGB Palette Errors");
            gtk_box_pack_start(GTK_BOX(vbox), check_ignore_palette_errors, false, false, 2);
            gtk_widget_show(check_ignore_palette_errors);
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_ignore_palette_errors),
                                         p_plugin_options->ignore_palette_errors);


        // Repair palette errors works by way of remapping the image to it's own palette with
        // a best-fit per-tile sub-palette restriction
        check_repair_palette_errors = gtk_check_button_new_with_label("Remap/Repair Palette Errors");
            gtk_box_pack_start(GTK_BOX(vbox), check_repair_palette_errors, false, false, 2);
            gtk_widget_show(check_repair_palette_errors);
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_repair_palette_errors),
                                         p_plugin_options->remap_pal);



        // Repair palette errors works by way of remapping the image to it's own palette with
        // a best-fit per-tile sub-palette restriction
        check_use_remap_pal_file = gtk_check_button_new_with_label("Remap using a Palette File");
            gtk_box_pack_start(GTK_BOX(vbox), check_use_remap_pal_file, false, false, 2);
            gtk_widget_show(check_use_remap_pal_file);
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_use_remap_pal_file),
                                         (p_plugin_options->remap_pal_file[0] != '\0'));



        label_dedupe = gtk_label_new("\nDeduplicate Map Tiles on:\n\n(Note: If Tile Pattern dedupe is turned off, \nexport may create too many tiles and fail. )");
        gtk_box_pack_start(GTK_BOX(vbox), label_dedupe, false, false, 2);
        gtk_misc_set_alignment(GTK_MISC(label_dedupe), 0.0f, 0.5f); // Left-align
        gtk_widget_show(label_dedupe);


            // DMG/CGB Options
            check_dedupe_tiles = gtk_check_button_new_with_label("Tile Pattern");
                gtk_box_pack_start(GTK_BOX(vbox), check_dedupe_tiles, false, false, 2);
                gtk_widget_show(check_dedupe_tiles);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_tiles),
                                             p_plugin_options->tile_dedupe_enabled);

            // CGB Only options
            check_dedupe_on_flip = gtk_check_button_new_with_label("Flipped on X or Y (CGB only)");
                gtk_box_pack_start(GTK_BOX(vbox), check_dedupe_on_flip, false, false, 2);
                gtk_widget_show(check_dedupe_on_flip);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_on_flip),
                                             p_plugin_options->tile_dedupe_flips);


            check_dedupe_on_palette = gtk_check_button_new_with_label("Alternate Palette (CGB only)");
                gtk_box_pack_start(GTK_BOX(vbox), check_dedupe_on_palette, false, false, 2);
                gtk_widget_show(check_dedupe_on_palette);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_on_palette),
                                             p_plugin_options->tile_dedupe_palettes);



        // Set up UI event signals
        g_signal_connect(dialog, "response", G_CALLBACK(on_response), &response);
        g_signal_connect(dialog, "destroy",  G_CALLBACK(gtk_main_quit), NULL);

        g_signal_connect(G_OBJECT(check_ignore_palette_errors), "toggled",
                         G_CALLBACK(on_settings_checkbutton_changed), &(p_plugin_options->ignore_palette_errors));

        g_signal_connect(G_OBJECT(check_repair_palette_errors), "toggled",
                         G_CALLBACK(on_settings_checkbutton_changed), &(p_plugin_options->remap_pal));

        g_signal_connect(G_OBJECT(check_use_remap_pal_file), "toggled",
                         G_CALLBACK(on_settings_remap_pal_file_changed), p_plugin_options);

        g_signal_connect(G_OBJECT(combo_gb_mode), "changed",
                         G_CALLBACK(on_settings_gb_mode_combo_changed), &(p_plugin_options->gb_mode));

        g_signal_connect(G_OBJECT(combo_tilesize), "changed",
                         G_CALLBACK(on_settings_tilesize_combo_changed), p_plugin_options);

        g_signal_connect(G_OBJECT(check_dedupe_tiles), "toggled",
                         G_CALLBACK(on_settings_checkbutton_changed), &(p_plugin_options->tile_dedupe_enabled)); // instead pass &(options.dedupe_tiles)

        g_signal_connect(G_OBJECT(check_dedupe_on_flip), "toggled",
                         G_CALLBACK(on_settings_checkbutton_changed), &(p_plugin_options->tile_dedupe_flips));

        g_signal_connect(G_OBJECT(check_dedupe_on_palette), "toggled",
                         G_CALLBACK(on_settings_checkbutton_changed), &(p_plugin_options->tile_dedupe_palettes));

        // TODO: Above is spawning these errors:
        //       IA__gtk_main_quit: assertion 'main_loops != NULL' failed
        //       https://www.gtk.org/tutorial1.2/gtk_tut-2.html
        //       https://ubuntuforums.org/showthread.php?t=394399

        update_enabled_ui_controls();

        // Set "Export" button as default with focus
        // (allows user to press "Enter Key" to save without making changes)
        gtk_widget_grab_focus(gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK));


        // Show the dialog and run it
        gtk_widget_show(dialog);
        gimp_dialog_run(GIMP_DIALOG(dialog));

        gtk_widget_destroy(dialog);


    log_verbose("Dialog Exit with %d\n",response);
    return response;
}


static void combo_set_active_entry_by_string(GtkWidget * combo, gchar * string_to_match) {

    GtkTreeModel       *model;
    GtkTreeIter         iter;
    gchar * selected_string;

    model = gtk_combo_box_get_model (GTK_COMBO_BOX(combo));

    if (gtk_tree_model_get_iter_first (model, &iter)) {

        do {

            gtk_combo_box_set_active_iter (GTK_COMBO_BOX(combo), &iter);
            selected_string = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

            // Exit loop if a match was found
            if (!(g_strcmp0(selected_string, string_to_match)))
                break;

        } while (gtk_tree_model_iter_next (model, &iter));
    }
}


static void on_settings_gb_mode_combo_changed(GtkComboBox *combo, gpointer callback_data)
{
    gchar * selected_string;

    selected_string = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

    // Match the string up to an output mode
    if (!(g_strcmp0(selected_string, STR_OPT_GB_MODE_DMG))) {

        *((uint16_t *)callback_data) = MODE_DMG_4_COLOR;
    }
    else if (!(g_strcmp0(selected_string, STR_OPT_GB_MODE_CGB))) {

        *((uint16_t *)callback_data) = MODE_CGB_32_COLOR;
    }
    // else *((uint16_t *)callback_data) = -1; // Signal error

    update_enabled_ui_controls();
}


static void on_settings_tilesize_combo_changed(GtkComboBox *combo, gpointer callback_data)
{
    gchar * selected_string;

    selected_string = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

    // Match the string up to an output mode
    if (!(g_strcmp0(selected_string, STR_OPT_TILE_SIZE_8x8))) {
        ((tile_process_options *)callback_data)->tile_width = 8;
        ((tile_process_options *)callback_data)->tile_height = 8;
    }
    else if (!(g_strcmp0(selected_string, STR_OPT_TILE_SIZE_8x16))) {
        ((tile_process_options *)callback_data)->tile_width = 8;
        ((tile_process_options *)callback_data)->tile_height = 16;
    }
    else if (!(g_strcmp0(selected_string, STR_OPT_TILE_SIZE_16x16))) {
        ((tile_process_options *)callback_data)->tile_width = 16;
        ((tile_process_options *)callback_data)->tile_height = 16;
    }
    else if (!(g_strcmp0(selected_string, STR_OPT_TILE_SIZE_32x32))) {
        ((tile_process_options *)callback_data)->tile_width = 32;
        ((tile_process_options *)callback_data)->tile_height = 32;
    }
    // update_enabled_ui_controls();
}


static void on_settings_checkbutton_changed(GtkToggleButton * p_togglebutton, gpointer callback_data) {

    // Update the variable that was passed in using the state of the calling check button
    *((uint16_t *)callback_data) = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_togglebutton));

    update_enabled_ui_controls();
}


static void update_enabled_ui_controls(void) {

    //         Dedupe-Tile  Dedupe-Flip  Dedupe-Palette   Ignore-Palette-Errors
    // DMG     Enabled         NO            NO               xx NO xx -> Enabled (changed to allow)
    // CGB     Enabled       Enabled      Enabled              Enabled

    // First disable any dedupe options except for GBM mode
    gtk_widget_set_sensitive((GtkWidget *) check_dedupe_on_flip, p_plugin_options->image_format == FORMAT_GBM);
    if (p_plugin_options->image_format != FORMAT_GBM)
        p_plugin_options->tile_dedupe_enabled = (p_plugin_options->image_format == FORMAT_GBM);

    // Now set available options based on DMG/CGB
    if (p_plugin_options->gb_mode == MODE_DMG_4_COLOR) {

        gtk_widget_set_sensitive((GtkWidget *) check_ignore_palette_errors, true); //false);

        gtk_widget_set_sensitive((GtkWidget *) check_repair_palette_errors, true);
        // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_repair_palette_errors), false);

        gtk_widget_set_sensitive((GtkWidget *) check_dedupe_on_flip, false);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_on_flip), false);

        gtk_widget_set_sensitive((GtkWidget *) check_dedupe_on_palette, false);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_on_palette), false);
    }
    else if (p_plugin_options->gb_mode == MODE_CGB_32_COLOR) {

        gtk_widget_set_sensitive((GtkWidget *) check_ignore_palette_errors, true);
        gtk_widget_set_sensitive((GtkWidget *) check_repair_palette_errors, true);

        gtk_widget_set_sensitive((GtkWidget *) check_dedupe_on_flip, p_plugin_options->tile_dedupe_enabled == true);
        gtk_widget_set_sensitive((GtkWidget *) check_dedupe_on_palette, p_plugin_options->tile_dedupe_enabled == true);
    }


    // Make sure remap checkboxs are in sync with the option requirenments

    // If pal file string is empty, turn off it's check box
    if (p_plugin_options->remap_pal_file[0] != '\0') {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_use_remap_pal_file), true);
    }
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_use_remap_pal_file))) {
        // If pal file checkbox is on and file string is populated
        // then turn on Palette Remapping/Repair to go along with the file
        p_plugin_options->remap_pal = true;
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_repair_palette_errors), p_plugin_options->remap_pal);
    }

    // Turn off the palette file check box and clear the file string if remapping is disabled
    if (!p_plugin_options->remap_pal) {
        p_plugin_options->remap_pal_file[0] != '\0';
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_use_remap_pal_file), false);
    }
}


static void on_settings_remap_pal_file_changed(GtkToggleButton * p_togglebutton, gpointer callback_data) {

    GtkWidget * file_dialog;
    gint res;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_togglebutton))) {

        file_dialog = gtk_file_chooser_dialog_new (
                        "Open Palette File (#RRGGBB text format)",
                        GTK_WINDOW (dialog),
                        GTK_FILE_CHOOSER_ACTION_OPEN,
                        GTK_STOCK_CANCEL,
                        GTK_RESPONSE_CANCEL,
                        GTK_STOCK_OPEN,
                        GTK_RESPONSE_ACCEPT,
                        NULL);//(void *) 0);

        res = gtk_dialog_run (GTK_DIALOG (file_dialog));
        if (res == GTK_RESPONSE_ACCEPT)
        {
            char *filename;
            GtkFileChooser *chooser = GTK_FILE_CHOOSER (file_dialog);
            filename = gtk_file_chooser_get_filename (chooser);
            strncpy( ((tile_process_options *)callback_data)->remap_pal_file, // Dest
                     filename,                                               // Src
                     STR_FILENAME_MAX);
            // // Turn on palette repair -> now handled in update_enabled_ui_controls()
            // p_plugin_options->remap_pal = true;
            // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_repair_palette_errors), p_plugin_options->remap_pal);
        } else {
            // Clear checkbox and filename if something went wrong
            ((tile_process_options *)callback_data)->remap_pal_file[0] = '\0';
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_togglebutton), false);
        }
        gtk_widget_destroy (file_dialog);

    } else {
        // Zero out the filename string
        ((tile_process_options *)callback_data)->remap_pal_file[0] = '\0';
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p_togglebutton), false);
    }

    update_enabled_ui_controls();
}


// TODO: Set control defaults
// Update enable/disable (TODO: THIS GOES IN A FUNCTION
        // GBR Export mode :
            // All map options are off
        // GBR Export mode : map options are off

// Colors
//         DMG mode   CGB mode
// 0- 4    Enabled    Enabled
// 0-32      NO       Enabled
// 32+       NO         NO
//
//
// File Type
//        gb_mode   map options
//GBR     Enabled      NO
//GBM     Enabled   Enabled
//
//
// gb_mode
//        Dedupe-Tile  Dedupe-Flip  Dedupe-Palette
//DMG     Enabled         NO            NO
//CGB     Enabled       Enabled      Enabled
//
//
// Enable everything
// if (colors < 32)
//     disable everything, warn ("Too many colors")
//
// if (colors > 4)
//     disable DMG output
//
// if (file_type == GBR)
//     disable all map dedupe options
//
// if (gb_mode == DMG)
//     disable dedupe on flip
//     disable dedupe on palette

