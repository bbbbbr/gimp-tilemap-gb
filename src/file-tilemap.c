//
// file-tilemap.c
//

#include "logging.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "tilemap_write.h"
#include "tilemap_read.h"
#include "tilemap_error.h"

#include "lib_tilemap.h"

#include "export-dialog.h"

const char SAVE_PROCEDURE_TMAP_C_SOURCE[] = "file-save-tilemap-c-source";
const char SAVE_PROCEDURE_GBR[] = "file-save-gbr";
const char LOAD_PROCEDURE_GBR[] = "file-load-gbr";
const char SAVE_PROCEDURE_GBM[] = "file-save-gbm";
const char LOAD_PROCEDURE_GBM[] = "file-load-gbm";

const char BINARY_NAME[]    = "file-tilemap";

// Predeclare our entrypoints
static void query(void);
static void run(const gchar *, gint, const GimpParam *, gint *, GimpParam **);
static int plugin_get_image_format_from_string(const gchar * plugin_procedure_name);

// Load arguments
static const GimpParamDef load_arguments[] =
{
    { GIMP_PDB_INT32,  "run-mode",     "Interactive, non-interactive" },
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_STRING, "raw-filename", "The name entered" }
};

// Load return values
static const GimpParamDef load_return_values[] =
{
    { GIMP_PDB_IMAGE, "image", "Output image" }
};

// Save arguments - should match if (nparams != 10)
static const GimpParamDef save_arguments[] =
{
    { GIMP_PDB_INT32,    "run-mode",     "Interactive, non-interactive" },
    { GIMP_PDB_IMAGE,    "image",        "Input image" },
    { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
    { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
    { GIMP_PDB_STRING,   "raw-filename", "The name entered" },
//        { GIMP_PDB_FLOAT,    "image_format",  "Tilemap export format" }
    { GIMP_PDB_INT16,    "gb_mode",              "Export type DMG 4 color or CGB 32 color" },
    { GIMP_PDB_INT16,    "tile_dedupe_enabled",  "Deduplicate Tiles on Pattern" },
    { GIMP_PDB_INT16,    "tile_dedupe_flips",    "Deduplicate Tiles on Flipped on X or Y (CGB only)" },
    { GIMP_PDB_INT16,    "tile_dedupe_palettes", "Deduplicate Tiles on Alternate Palette (CGB only)" },
    { GIMP_PDB_INT16,    "tile_width",           "Tile Width" },
    { GIMP_PDB_INT16,    "tile_height",          "Tile Height" },
    { GIMP_PDB_INT16,    "ignore_palette_errors", "Ignore Palette Errors" },
    { GIMP_PDB_INT16,    "repair_palette_errors", "Repair Palette Errors" },
    { GIMP_PDB_INT16,    "palette_tile_width",    "Tile Palette Width" },
    { GIMP_PDB_INT16,    "palette_tile_height",   "Tile palette Height" },
};


// Declare our plugin entry points
GimpPlugInInfo PLUG_IN_INFO = {
    NULL,
    NULL,
    query,
    run
};

MAIN()

// The query function
static void query(void)
{
    // Install the load procedure for ".GBR" files
    gimp_install_procedure(LOAD_PROCEDURE_GBR,
                           "Load GBR Game Boy tileset",
                           "Load GBR Game Boy tileset",
                           "--",
                           "Copyright --",
                           "2019",
                           "GBR Game Boy tileset image",
                           NULL,
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(load_arguments),
                           G_N_ELEMENTS(load_return_values),
                           load_arguments,
                           load_return_values);


    // Install the load procedure for ".GBM" files
    gimp_install_procedure(LOAD_PROCEDURE_GBM,
                           "Load GBM Game Boy tileset map",
                           "Load GBM Game Boy tileset map",
                           "--",
                           "Copyright --",
                           "2019",
                           "GBM Game Boy tileset map image",
                           NULL,
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(load_arguments),
                           G_N_ELEMENTS(load_return_values),
                           load_arguments,
                           load_return_values);


    // Install the save procedure for ".TMAP" files (all formats)
    gimp_install_procedure(SAVE_PROCEDURE_TMAP_C_SOURCE,
                           "Save image to tilemap",
                           "Save image to tilemap",
                           "--",
                           "Copyright --",
                           "2019",
                           "Tilemap & Tileset",
                           "INDEXED*, RGB*", // , GREYSCALE*", // TODO
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(save_arguments),
                           0,
                           save_arguments,
                           NULL);

    // Install the save procedure for ".GBR" files (all formats)
    gimp_install_procedure(SAVE_PROCEDURE_GBR,
                           "Save GBR Game Boy tileset",
                           "Save GBR Game Boy tileset",
                           "--",
                           "Copyright --",
                           "2019",
                           "GBR Game Boy tileset image",
                           "INDEXED*, RGB*", // , GREYSCALE*", // TODO
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(save_arguments),
                           0,
                           save_arguments,
                           NULL);

    // Install the save procedure for ".GBM" files (all formats)
    gimp_install_procedure(SAVE_PROCEDURE_GBM,
                           "Save GBM Game Boy tileset map",
                           "Save GBM Game Boy tileset map",
                           "--",
                           "Copyright --",
                           "2019",
                           "GBM Game Boy tileset map image",
                           "INDEXED*, RGB*", // , GREYSCALE*", // TODO
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(save_arguments),
                           0,
                           save_arguments,
                           NULL);

    // Register load handlers
    gimp_register_load_handler(LOAD_PROCEDURE_GBR, "gbr", "");
    gimp_register_load_handler(LOAD_PROCEDURE_GBM, "gbm", "");

    // Now register the save handlers
    gimp_register_save_handler(SAVE_PROCEDURE_TMAP_C_SOURCE, "tmap", "");
    gimp_register_save_handler(SAVE_PROCEDURE_GBR, "gbr", "");
    gimp_register_save_handler(SAVE_PROCEDURE_GBM, "gbm", "");
}


// This is a shim since gimp_image_get_colormap_size() might not be available
static int colormap_size_get(int image_id) {
    guchar     * p_colormap_buf;
    gint         colormap_numcolors;
    colormap_numcolors = 0;
    p_colormap_buf = NULL;

    p_colormap_buf = gimp_image_get_colormap(image_id, &colormap_numcolors);

    if (p_colormap_buf == NULL)
        colormap_numcolors = 0;

    return colormap_numcolors;
}


static int plugin_get_image_format_from_string(const gchar * plugin_procedure_name) {

    int image_format;
    image_format = -1;

    if (!strcmp(plugin_procedure_name, SAVE_PROCEDURE_TMAP_C_SOURCE))
        image_format = FORMAT_GBDK_C_SOURCE;

    else if ((!strcmp(plugin_procedure_name, SAVE_PROCEDURE_GBR)) ||
             (!strcmp(plugin_procedure_name, LOAD_PROCEDURE_GBR)))
        image_format = FORMAT_GBR;

    else if ((!strcmp(plugin_procedure_name, SAVE_PROCEDURE_GBM)) ||
             (!strcmp(plugin_procedure_name, LOAD_PROCEDURE_GBM)) )
        image_format = FORMAT_GBM;

    return image_format;
}


// The run function
static void run(const gchar * plugin_procedure_name,
         gint nparams,
         const GimpParam * param,
         gint * nreturn_vals,
         GimpParam ** return_vals)
{
    // Create the return value.
    static GimpParam return_values[2];
    GimpRunMode   run_mode;
    tile_process_options plugin_options;

    options_reset(&plugin_options);


    *nreturn_vals = 1;
    *return_vals  = return_values;
    run_mode      = param[0].data.d_int32;

    // log_set_level(OUTPUT_LEVEL_VERBOSE);
    // log_set_level(OUTPUT_LEVEL_ONLY_ERRORS);
    log_set_level(OUTPUT_LEVEL_DEFAULT);

    // Set the return value to success by default
    return_values[0].type          = GIMP_PDB_STATUS;
    return_values[0].data.d_status = GIMP_PDB_SUCCESS;

    plugin_options.image_format = plugin_get_image_format_from_string(plugin_procedure_name);
    log_verbose("image_format=%d\n", plugin_options.image_format);
    log_verbose("plugin_procedure_name=%s\n", plugin_procedure_name);


    tilemap_error_clear();


// Check to see if this is the load procedure
    if( (!strcmp(plugin_procedure_name, LOAD_PROCEDURE_GBR)) ||
        (!strcmp(plugin_procedure_name, LOAD_PROCEDURE_GBM)) )
    {
        int new_image_id;

        // Check to make sure all parameters were supplied
        if(nparams != 3) {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }


        // Initialize UI for any spawned dialogs
        gimp_ui_init(BINARY_NAME, false);

        // Now read the image
        new_image_id = tilemap_read(param[1].data.d_string, plugin_options.image_format);

        // Check for an error
        if(new_image_id == -1)
        {
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
            return;
        }

        // Fill in the second return value
        *nreturn_vals = 2;

        return_values[1].type         = GIMP_PDB_IMAGE;
        return_values[1].data.d_image = new_image_id;
    }
    else if( (!strcmp(plugin_procedure_name, SAVE_PROCEDURE_TMAP_C_SOURCE)) ||
             (!strcmp(plugin_procedure_name, SAVE_PROCEDURE_GBR)) ||
             (!strcmp(plugin_procedure_name, SAVE_PROCEDURE_GBM)))

    {
        // === This is the export procedure ===

        gint32 image_id, drawable_id;
        int status = 1;
        GimpExportReturn export_ret;

        // Check to make sure all of the parameters were supplied
        if (nparams != (sizeof(save_arguments) / sizeof(save_arguments[0]))) // Should match save_arguments[]
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        image_id    = param[1].data.d_int32;
        drawable_id = param[2].data.d_int32;
        // plugin_options.image_format is set above


        // Initialize UI for any spawned dialogs
        gimp_ui_init(BINARY_NAME, false);


        // Handle the different run modes
        switch (run_mode) {

            case GIMP_RUN_INTERACTIVE:
                // Only pop up the export dialog if it's interactive mode
                // Set defaults first
                gimp_get_data (plugin_procedure_name, &plugin_options);

                options_color_defaults_if_unset(colormap_size_get(image_id), &plugin_options);

                // Prompt the user for export options in a dialog
                // // Allow user to override the defaults
                if (!export_dialog(&plugin_options, plugin_procedure_name) ) {
                    // Abort plugin if user canceled in the export dialog
                    return_values[0].data.d_status = GIMP_PDB_CANCEL;
                    return;
                }

                // Re-apply any changes to the defaults
                tilemap_options_set(&plugin_options);
                break;

            case GIMP_RUN_NONINTERACTIVE:
                // Read in non-interactive mode plug settings, then apply them
                // Load export options
                plugin_options.gb_mode              = param[6].data.d_int16;
                plugin_options.tile_dedupe_enabled  = param[7].data.d_int16;
                plugin_options.tile_dedupe_flips    = param[8].data.d_int16;
                plugin_options.tile_dedupe_palettes = param[9].data.d_int16;
                plugin_options.tile_width           = param[10].data.d_int16;
                plugin_options.tile_height          = param[11].data.d_int16;
                plugin_options.ignore_palette_errors = param[12].data.d_int16;
                plugin_options.remap_pal             = param[13].data.d_int16;
                plugin_options.palette_tile_width    = param[14].data.d_int16;
                plugin_options.palette_tile_height   = param[15].data.d_int16;

                tilemap_options_set(&plugin_options);
                break;

            case GIMP_RUN_WITH_LAST_VALS:

                //  Try to retrieve plugin settings, then apply them
                gimp_get_data (plugin_procedure_name, &plugin_options);
                tilemap_options_set(&plugin_options);
                break;
        }

        log_verbose("Handling export format: %d\n", plugin_options.image_format);

        // TODO: consolidate different format handling below

        // Determine image file format by load type
        // Prepare app image for export in the desired format
        switch (plugin_options.image_format) {
            case FORMAT_GBDK_C_SOURCE:
                export_ret = gimp_export_image(&image_id,
                                               &drawable_id,
                                               "TMAP",
                                               // GIMP_EXPORT_CAN_HANDLE_GRAY | // TODO
                                               GIMP_EXPORT_CAN_HANDLE_RGB |
                                               GIMP_EXPORT_CAN_HANDLE_INDEXED |
                                               GIMP_EXPORT_CAN_HANDLE_ALPHA);
                break;

            case FORMAT_GBR:
                // TODO: ALPHA SUPPORT FOR GBR ?
                export_ret = gimp_export_image(&image_id,
                                               &drawable_id,
                                               "GBR",
                                               // GIMP_EXPORT_CAN_HANDLE_GRAY | // TODO
                                               GIMP_EXPORT_CAN_HANDLE_RGB |
                                               GIMP_EXPORT_CAN_HANDLE_INDEXED |
                                               GIMP_EXPORT_CAN_HANDLE_ALPHA);
                break;

            case FORMAT_GBM:
                // TODO: ALPHA SUPPORT FOR GBM ?
                export_ret = gimp_export_image(&image_id,
                                               &drawable_id,
                                               "GBM",
                                               // GIMP_EXPORT_CAN_HANDLE_GRAY | // TODO
                                               GIMP_EXPORT_CAN_HANDLE_RGB |
                                               GIMP_EXPORT_CAN_HANDLE_INDEXED |
                                               GIMP_EXPORT_CAN_HANDLE_ALPHA);
                break;

            default:
                log_error("Invalid export format: %d\n", plugin_options.image_format);
                return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
                return;
                break;
        }


        log_verbose("gimp_export_image: returned:%d (GIMP_EXPORT_EXPORT=%d, GIMP_EXPORT_IGNORE=%d, GIMP_EXPORT_CANCEL=%d)\n", export_ret,
                     GIMP_EXPORT_EXPORT, GIMP_EXPORT_IGNORE);

        switch(export_ret)
        {
            case GIMP_EXPORT_EXPORT:
            case GIMP_EXPORT_IGNORE:

                // This is the main call to convert and save the image
                // in the desired output format
                status = write_tilemap(param[3].data.d_string,
                                     image_id,
                                     drawable_id,
                                     plugin_procedure_name);

                gimp_image_delete(image_id);

                break;

            case GIMP_EXPORT_CANCEL:
                return_values[0].data.d_status = GIMP_PDB_CANCEL;
                return;
                break;
        }

        gimp_set_data (plugin_procedure_name, &plugin_options, sizeof(tile_process_options));

        if(!status) {
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;

            if (tilemap_error_get() != TILE_ID_OK) {
                gimp_message(tilemap_error_get_string());
            }
        }

    } // End === SAVE PROCEDURE ===
    else
        return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;

}
