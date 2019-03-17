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


#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

// #include "lib_rom_bin.h"
#include "write-tilemap.h"
// #include "export-dialog.h"

const char SAVE_PROCEDURE[] = "file-tilemap-save";

const char BINARY_NAME[]    = "file-tilemap";

// Predeclare our entrypoints
static void query(void);
static void run(const gchar *, gint, const GimpParam *, gint *, GimpParam **);

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

    // Install the save procedure for ".tmap" files (all formats)
    gimp_install_procedure(SAVE_PROCEDURE,
                           "Save image to tilemap",
                           "Save image to tilemap",
                           "--",
                           "Copyright --",
                           "2018",
                           "Tilemap & Tileset",
                           "INDEXED*",
                           GIMP_PLUGIN,
                           G_N_ELEMENTS(save_arguments),
                           0,
                           save_arguments,
                           NULL);


    // Now register the save handlers
    gimp_register_save_handler(SAVE_PROCEDURE, "tmap", "");
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

    // Set the return value to success by default
    return_values[0].type          = GIMP_PDB_STATUS;
    return_values[0].data.d_status = GIMP_PDB_SUCCESS;

// printf("file-tilemap.c: Start\n");

    if(!strcmp(name, SAVE_PROCEDURE))
    {
        // This is the export procedure

        gint32 image_id, drawable_id;
        int status = 1;
        int image_mode = -1;
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
        export_ret = gimp_export_image(&image_id,
                                       &drawable_id,
                                       "TMAP",
                                       GIMP_EXPORT_CAN_HANDLE_INDEXED |
                                       GIMP_EXPORT_CAN_HANDLE_ALPHA);

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
              // TODO: Export function here
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
