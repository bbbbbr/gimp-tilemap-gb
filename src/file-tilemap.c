//
// file-tilemap.c
//


#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

// #include "lib_rom_bin.h"
#include "tilemap_write.h"
#include "tilemap_read.h"

#include "lib_tilemap.h"

// #include "export-dialog.h"

const char SAVE_PROCEDURE_TMAP_C_SOURCE[] = "file-save-tilemap-c-source";
const char SAVE_PROCEDURE_GBR[] = "file-save-gbr";
const char LOAD_PROCEDURE_GBR[] = "file-load-gbr";
const char SAVE_PROCEDURE_GBM[] = "file-save-gbm";
const char LOAD_PROCEDURE_GBM[] = "file-load-gbm";

const char BINARY_NAME[]    = "file-tilemap";

// Predeclare our entrypoints
static void query(void);
static void run(const gchar *, gint, const GimpParam *, gint *, GimpParam **);
static int plugin_get_image_mode_from_string(const gchar * name);

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

    // Save arguments
    static const GimpParamDef save_arguments[] =
    {
        { GIMP_PDB_INT32,    "run-mode",     "Interactive, non-interactive" },
        { GIMP_PDB_IMAGE,    "image",        "Input image" },
        { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
        { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
        { GIMP_PDB_STRING,   "raw-filename", "The name entered" },
        { GIMP_PDB_FLOAT,    "export_mode",  "Tilemap export format" }
    };

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
                           "INDEXED*",
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
                           "INDEXED*",
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
                           "INDEXED*",
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



static int plugin_get_image_mode_from_string(const gchar * name) {

    int image_mode;
    image_mode = -1;

    if (!strcmp(name, SAVE_PROCEDURE_TMAP_C_SOURCE))
        // (!strcmp(name, LOAD_PROCEDURE_TMAP_C_SOURCE))
        image_mode = EXPORT_FORMAT_GBDK_C_SOURCE;

    else if ((!strcmp(name, SAVE_PROCEDURE_GBR)) ||
             (!strcmp(name, LOAD_PROCEDURE_GBR)))
        image_mode = EXPORT_FORMAT_GBR;

    else if ((!strcmp(name, SAVE_PROCEDURE_GBM)) ||
             (!strcmp(name, LOAD_PROCEDURE_GBM)) )
        image_mode = EXPORT_FORMAT_GBM;

    return image_mode;
}


// The run function
static void run(const gchar * name,
         gint nparams,
         const GimpParam * param,
         gint * nreturn_vals,
         GimpParam ** return_vals)
{
    // Create the return value.
    static GimpParam return_values[2];
    *nreturn_vals = 1;
    *return_vals  = return_values;

    GimpRunMode   run_mode;
    run_mode      = param[0].data.d_int32;
    int           image_mode;

    // Set the return value to success by default
    return_values[0].type          = GIMP_PDB_STATUS;
    return_values[0].data.d_status = GIMP_PDB_SUCCESS;

    image_mode = plugin_get_image_mode_from_string(name);

    printf("image_mode=%d\n", image_mode);

// Check to see if this is the load procedure
    if( (!strcmp(name, LOAD_PROCEDURE_GBR)) ||
        (!strcmp(name, LOAD_PROCEDURE_GBM)) )
    {
        int new_image_id;

        // Check to make sure all parameters were supplied
        if(nparams != 3) {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }


        // Try to export the image
        gimp_ui_init(BINARY_NAME, FALSE);

        // Now read the image
        new_image_id = tilemap_read(param[1].data.d_string, image_mode);

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
    else if( (!strcmp(name, SAVE_PROCEDURE_TMAP_C_SOURCE)) ||
             (!strcmp(name, SAVE_PROCEDURE_GBR)) ||
             (!strcmp(name, SAVE_PROCEDURE_GBM)))

    {
        // This is the export procedure

        gint32 image_id, drawable_id;
        int status = 1;
        GimpExportReturn export_ret;

        // Check to make sure all of the parameters were supplied
        if(nparams != 6)
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        image_id    = param[1].data.d_int32;
        drawable_id = param[2].data.d_int32;


        printf("file-tilemap.c: Export\n");

        // Try to export the image
        gimp_ui_init(BINARY_NAME, FALSE);

        printf("file-tilemap.c: Call Export\n");


        // TODO: consolidate different format handling below

        // Determine image file format by load type
        // Prepare app image for export in the desired format
        // (if it's not indexed this will auto-convert it to that)
        switch (image_mode) {
            case EXPORT_FORMAT_GBDK_C_SOURCE:
                export_ret = gimp_export_image(&image_id,
                                               &drawable_id,
                                               "TMAP",
                                               GIMP_EXPORT_CAN_HANDLE_INDEXED |
                                               GIMP_EXPORT_CAN_HANDLE_ALPHA);
                break;

            case EXPORT_FORMAT_GBR:
                // TODO: ALPHA SUPPORT FOR GBR ?
                export_ret = gimp_export_image(&image_id,
                                               &drawable_id,
                                               "GBR",
                                               GIMP_EXPORT_CAN_HANDLE_INDEXED);
                break;

            case EXPORT_FORMAT_GBM:
                // TODO: ALPHA SUPPORT FOR GBM ?
                export_ret = gimp_export_image(&image_id,
                                               &drawable_id,
                                               "GBM",
                                               GIMP_EXPORT_CAN_HANDLE_INDEXED);
                break;
        }


        printf("export_ret=%d\n", export_ret);

        switch(export_ret)
        {
            case GIMP_EXPORT_EXPORT:
            case GIMP_EXPORT_IGNORE:

/*
              // TODO: Now get the settings via a pop-up dialog
              if(!export_dialog(&image_mode, name))
              {
                  return_values[0].data.d_status = GIMP_PDB_CANCEL;
                  return;
              }
*/
              // This is the main call to convert and save the image
              // in the desired output format
              status = write_tilemap(param[3].data.d_string,
                                     image_id,
                                     drawable_id,
                                     image_mode);

              gimp_image_delete(image_id);

              break;

            case GIMP_EXPORT_CANCEL:
                return_values[0].data.d_status = GIMP_PDB_CANCEL;
                return;
        }

        if(!status)
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    }
    else
        return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
}
