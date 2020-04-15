/*=======================================================================
              ROM bin load / save plugin for the GIMP
                 Copyright 2018 - Others & Nathan Osman (webp plugin base)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
=======================================================================*/

// TODO: rename to settings-dialog.c/h

#include "export-dialog.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

extern const char SAVE_PROCEDURE[];
extern const char LOAD_PROCEDURE[];
extern const char BINARY_NAME[];

// These should match the enumeration of tile_process_modes
const char STR_OPT_GB_MODE_DMG[] = "DMG 4 Color";
const char STR_OPT_GB_MODE_CGB[] = "CGB 32 Color";


void on_response(GtkDialog *, gint, gpointer);
static void on_settings_gb_mode_combo_changed(GtkComboBox *combo, gpointer callback_data);
static void on_settings_checkbutton_changed(GtkToggleButton * p_togglebutton, gpointer callback_data);


void on_response(GtkDialog * dialog, gint response_id, gpointer user_data) {

    // Quit the loop
    gtk_main_quit(); // TODO: delete?

    // Capture and map the response ID to a return code
    if (response_id == GTK_RESPONSE_OK)
        *((int *)user_data) = true; // pointer to response var in dialog
    // *((int *)user_data) = response_id;
}


int export_dialog(tile_process_options * p_export_options, const char * name) {

    int response = false;

    GtkWidget * dialog;
    GtkWidget * vbox;
    GtkWidget * label;
    GtkWidget * label_dedupe;

    GtkWidget * combo_gb_mode;
    GtkWidget * check_dedupe_tiles;
    GtkWidget * check_dedupe_on_flip;
    GtkWidget * check_dedupe_on_palette;


    // Create the export dialog

    // If Exporting use the export dialog convenience function
    dialog = gimp_export_dialog_new("GBTD/GBMB",
                                    BINARY_NAME,
                                    name);//SAVE_PROCEDURE);

    // Create the VBox
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                       vbox, TRUE, TRUE, 2);
    gtk_widget_show(vbox);

    // Create the label
    label = gtk_label_new("Game Boy Tile Set / Tile Map Export options.");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 2);
    gtk_widget_show(label);


    // Create a combo/list box for selecting the mode
    combo_gb_mode = gtk_combo_box_text_new();
    // Add the mode select entries
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_gb_mode), STR_OPT_GB_MODE_DMG);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_gb_mode), STR_OPT_GB_MODE_CGB);

    // Select default value
    // TODO: try to auto-detect image mode based on number of colors? (export only)
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_gb_mode), p_export_options->gb_mode);
    // Add it to the box for display and show it
    gtk_box_pack_start(GTK_BOX(vbox), combo_gb_mode, FALSE, FALSE, 2);
    gtk_widget_show(combo_gb_mode);


    // == Map export options ==

    label_dedupe = gtk_label_new("\nDeduplicate Map Tiles on:");
    gtk_box_pack_start(GTK_BOX(vbox), label_dedupe, FALSE, FALSE, 2);
    gtk_widget_show(label_dedupe);

    // DMG/CGB Options
    check_dedupe_tiles = gtk_check_button_new_with_label("Tile Pattern");
        gtk_box_pack_start(GTK_BOX(vbox), check_dedupe_tiles, FALSE, FALSE, 2);
        gtk_widget_show(check_dedupe_tiles);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_tiles),
                                     p_export_options->tile_dedupe_enabled);

    // CGB Only options
    check_dedupe_on_flip = gtk_check_button_new_with_label("Flipped on X or Y (CGB only)");
        gtk_box_pack_start(GTK_BOX(vbox), check_dedupe_on_flip, FALSE, FALSE, 2);
        gtk_widget_show(check_dedupe_on_flip);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_on_flip),
                                     p_export_options->tile_dedupe_flips);


    check_dedupe_on_palette = gtk_check_button_new_with_label("Alternate Palette (CGB only)");
        gtk_box_pack_start(GTK_BOX(vbox), check_dedupe_on_palette, FALSE, FALSE, 2);
        gtk_widget_show(check_dedupe_on_palette);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_dedupe_on_palette),
                                     p_export_options->tile_dedupe_palettes);



    // Set up UI event signals
    g_signal_connect(dialog, "response", G_CALLBACK(on_response), &response);
    g_signal_connect(dialog, "destroy",  G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(combo_gb_mode), "changed",
                     G_CALLBACK(on_settings_gb_mode_combo_changed), &(p_export_options->gb_mode));

    g_signal_connect(G_OBJECT(check_dedupe_tiles), "toggled",
                     G_CALLBACK(on_settings_checkbutton_changed), &(p_export_options->tile_dedupe_enabled)); // instead pass &(options.dedupe_tiles)

    g_signal_connect(G_OBJECT(check_dedupe_on_flip), "toggled",
                     G_CALLBACK(on_settings_checkbutton_changed), &(p_export_options->tile_dedupe_flips));

    g_signal_connect(G_OBJECT(check_dedupe_on_palette), "toggled",
                     G_CALLBACK(on_settings_checkbutton_changed), &(p_export_options->tile_dedupe_palettes));

    // TODO: Above is spawning these errors:
    //       IA__gtk_main_quit: assertion 'main_loops != NULL' failed
    //       https://www.gtk.org/tutorial1.2/gtk_tut-2.html
    //       https://ubuntuforums.org/showthread.php?t=394399

    // Show the dialog and run it
    gtk_widget_show(dialog);
    gimp_dialog_run(GIMP_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    return response;
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
}


static void on_settings_checkbutton_changed(GtkToggleButton * p_togglebutton, gpointer callback_data) {

    // Update the variable that was passed in using the state of the calling check button
    *((uint16_t *)callback_data) = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p_togglebutton));
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
// File Type
//        gb_mode   map options
//GBR     Enabled      NO
//GBM     Enabled   Enabled
//
// gb_mode
//        Dedupe-Tile  Dedupe-Flip  Dedupe-Palette
//DMG     Enabled         NO            NO
//CGB     Enabled       Enabled      Enabled
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

