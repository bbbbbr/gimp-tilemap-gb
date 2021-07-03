// console_main.c

//
// Console front end which collects settings, then
// loads images and initiates tilemap conversion
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "logging.h"

#include "lib_tilemap.h"
#include "tilemap_error.h"
#include "tilemap_console.h"
#include "tilemap_path_ops.h"

#include "image_remap.h"


#define ARG_INPUT_FILE    1
#define ARG_OUTPUT_MODE   2
#define ARG_OPTIONS_START 3

const char * const opt_gbr = "-gbr";
const char * const opt_gbm = "-gbm";
const char * const opt_csource   = "-csource";

const char * const opt_remap_pal   = "-pal=";

// user overrides for default settings
tile_process_options user_options;
char filename_in[STR_FILENAME_MAX] = {'\0'};
char filename_out[STR_FILENAME_MAX] = {'\0'};

int convert_image(void);
void apply_user_options(tile_process_options *);;
void clear_user_options(void);
int handle_args(int, char * []);
void display_help(void);


int main( int argc, char *argv[] )  {

    if (handle_args(argc, argv)) {

        if (convert_image()) {
            return 0; // Exit with Success
        }
    }

    return 1; // Exit with failure
}


int convert_image() {

    tile_process_options options;
    color_data src_colors;
    image_data src_image;
    src_image.p_img_data = NULL;

    // Process and export the image
    // Has to happen before tilemap_options_load_defaults()
    options.remap_pal      = user_options.remap_pal_file;
    strncpy(options.remap_pal_file, user_options.remap_pal_file, STR_FILENAME_MAX);
    tilemap_options_set(&options); // This is a workaround for now, should be split to separate options group
    if (!tilemap_load_and_prep_image(&src_image, &src_colors, filename_in ))
        return false;

    // Load default options based on output image format and number of colors in source image
    options.image_format = user_options.image_format;
    tilemap_options_load_defaults(src_colors.color_count, &options);

    // Apply any custom user settings from the command line
    apply_user_options(&options);
    // TODO: validate color count vs gb_mode

    // Apply the finalized options
    tilemap_options_set(&options);


    // Process and export the image
    if (!tilemap_process_and_save_image(&src_image, &src_colors, filename_out )) {

        if (tilemap_error_get() != TILE_ID_OK) {
            log_error("%s\n", tilemap_error_get_string() );
        }
        return false;
    }

}


void apply_user_options(tile_process_options * p_options) {

    if (user_options.gb_mode != OPTION_UNSET)
        p_options->gb_mode = user_options.gb_mode;

    if (user_options.tile_dedupe_enabled != OPTION_UNSET)
        p_options->tile_dedupe_enabled = user_options.tile_dedupe_enabled;

    if (user_options.tile_dedupe_flips != OPTION_UNSET)
        p_options->tile_dedupe_flips = user_options.tile_dedupe_flips;

    if (user_options.tile_dedupe_palettes != OPTION_UNSET)
        p_options->tile_dedupe_palettes = user_options.tile_dedupe_palettes;

    if (user_options.ignore_palette_errors != OPTION_UNSET)
        p_options->ignore_palette_errors = user_options.ignore_palette_errors;
}



void clear_user_options() {

    user_options.image_format          = OPTION_UNSET;
    user_options.gb_mode               = OPTION_UNSET;
    user_options.tile_dedupe_enabled   = OPTION_UNSET;
    user_options.tile_dedupe_flips     = OPTION_UNSET;
    user_options.tile_dedupe_palettes  = OPTION_UNSET;
    user_options.ignore_palette_errors = OPTION_UNSET;
}



int handle_args( int argc, char * argv[] ) {

    int i;
    char filename_noext[STR_FILENAME_MAX] = {'\0'};

    clear_user_options();

    if( argc < 3 ) {
        log_error("Error: At least two arguments are required\n\n");
        display_help();
        return false;
    }

    // Copy input filename
    strncpy(filename_in, argv[ARG_INPUT_FILE], STR_FILENAME_MAX);

    // Select output mode (from second argument)
    if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_gbr, sizeof(opt_gbr))) {
        user_options.image_format = FORMAT_GBR;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_gbm, sizeof(opt_gbr))) {
        user_options.image_format = FORMAT_GBM;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_csource, sizeof(opt_csource))) {
        user_options.image_format = FORMAT_GBDK_C_SOURCE;

    } else {

        log_error("Error: Output mode missing or incorrect\n\n");
        display_help();
        return false;
    }

    // Handle any remaining options
    // argc is zero based
    for (i = 3; i <= (argc -1); i++ ) {

        // Any argument that starts with a dash ('-') character
        // is an option, so process those first
        if (*argv[i] == '-') {

            // Multi char arguments
            if (strstr(argv[i], opt_remap_pal) == argv[i]) {
                // Extract filename for user supplier palette
                snprintf(user_options.remap_pal_file, STR_FILENAME_MAX, "%s", argv[i] + strlen(opt_remap_pal));
                user_options.remap_pal = true;
            }
            else {   

                // Single char arguments
                switch (*(argv[i]+1)) {
                    case 'g': user_options.gb_mode = MODE_DMG_4_COLOR;
                              break;
                    case 'c': user_options.gb_mode = MODE_CGB_32_COLOR;
                              break;

                    case 'd': user_options.tile_dedupe_enabled = false;
                              break;
                    case 'f': user_options.tile_dedupe_flips = false;
                              break;
                    case 'p': user_options.tile_dedupe_palettes = false;
                              break;
                    case 'i': user_options.ignore_palette_errors = true;
                              break;

                    case 'v': log_set_level(OUTPUT_LEVEL_VERBOSE);
                              break;
                    case 'e': log_set_level(OUTPUT_LEVEL_ONLY_ERRORS);
                              break;
                    case 'q': log_set_level(OUTPUT_LEVEL_QUIET);
                              break;
                }
            }
        } else {

            // Load output filename if specified
            if (filename_out[0] == '\0')
                strncpy(filename_out, argv[i], STR_FILENAME_MAX);
        }
    }

    // If output filename wasn't specified, then
    // try to set it based on the input filename
    if (filename_out[0] == '\0') {

        copy_filename_without_extension(filename_noext, argv[ARG_INPUT_FILE]);

        switch (user_options.image_format) {
            case FORMAT_GBDK_C_SOURCE:
                snprintf(&filename_out[0], STR_FILENAME_MAX, "%s%s",  &filename_noext[0], ".c");
                break;

            case FORMAT_GBR:
                snprintf(&filename_out[0], STR_FILENAME_MAX, "%s%s",  &filename_noext[0], ".gbr");
                break;

            case FORMAT_GBM:
                snprintf(&filename_out[0], STR_FILENAME_MAX, "%s%s",  &filename_noext[0], ".gbm");
                break;
        }
    }

    return true;
}


void display_help(void) {

    log_standard("Usage\n"
            "   png2gbtiles input_file.png -gbr|-gbm|-csource [options] [output_file]\n"
            "\n"
            "Options\n"
            "\n"
            "  -g          Force DMG color mode (4 colors or less only)\n"
            "  -c          Force CGB color mode (up to 32 colors)\n"
            "\n"
            "  -d          Turn OFF Map tile deduplication of tile PATTERN (.gbm only)\n"
            "  -f          Turn OFF Map tile deduplication of FLIP X/Y (.gbm only)\n"
            "  -p          Turn OFF Map tile deduplication of ALTERNATE PALETTE (.gbm only)\n"
            "\n"
            "  -i          Ignore Palette Errors (CGB will use highest guessed palette #)\n"
            "  -pal=[file] Remap png to palette (pngs allowed: index and 24/32 bit RGB)\n"
            "\n"
            "  -q          Quiet, suppress all output\n"
            "  -e          Errors only, suppress all non-error output\n"
            "  -v          Verbose output during conversion\n"
            "\n"
            "Examples\n"
            "   png2gbtiles spritesheet.png -gbr spritesheet.gbr\n"
            "   png2gbtiles worldmap.png -gbm -d -f -p worldmap.gbm\n"
            "   png2gbtiles worldmap.png -gbm \n");
            "Remap Palette format: RGB in hex text, 1 color per line (ex: FF0080)\n";
}