// console_main.c

#include <stdio.h>
#include <string.h>

#include "logging.h"

#include "lib_tilemap.h"
#include "tilemap_error.h"
#include "tilemap_console.h"
#include "tilemap_path_ops.h"


#define ARG_INPUT_FILE    1
#define ARG_OUTPUT_MODE   2
#define ARG_OPTIONS_START 3

const char * const opt_gbr = "-gbr";
const char * const opt_gbm = "-gbm";
const char * const opt_csource   = "-csource";

int convert_image( int, char * []);
void display_help(void);

int main( int argc, char *argv[] )  {

    if (convert_image(argc, argv)) {

        return 0; // Exit with Success
    } else {
        return 1; // Exit with failure
    }
}



int convert_image( int argc, char * argv[] ) {

    int i;
    char filename_out[STR_FILENAME_MAX] = {'\0'};
    char filename_noext[STR_FILENAME_MAX] = {'\0'};
    tile_process_options options;
    image_data src_image;
    color_data src_colors;


    if( argc < 3 ) {
        log_error("Error: At least two arguments are required\n\n");
        display_help();
        return false;
    }


    // Load source image (from first argument)
    if (!tilemap_load_image(&src_image, &src_colors, argv[ ARG_INPUT_FILE ])) {
        log_error("Error: Failed to load image\n\n");
        return false;
    }


// TODO: output_mode_set()
    // Select output mode (from second argument)
    if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_gbr, sizeof(opt_gbr))) {
        options.image_format = FORMAT_GBR;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_gbm, sizeof(opt_gbr))) {
        options.image_format = FORMAT_GBM;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_csource, sizeof(opt_csource))) {
        options.image_format = FORMAT_GBDK_C_SOURCE;

    } else {

        log_error("Error: Output mode missing or incorrect\n\n");
        display_help();
        return false;
    }
    // Load default options based on output image format
    tilemap_options_load_defaults(src_colors.color_count, &options);

// TODO: handle_options()
    // Handle any remaining options
    // argc is zero based
    for (i = 3; i <= (argc -1); i++ ) {

        // Any argument that starts with a dash ('-') character
        // is an option, so process those first
        if (*argv[i] == '-') {
            switch (*(argv[i]+1)) {
                case 'g': options.gb_mode = MODE_DMG_4_COLOR;
                          break;
                case 'c': options.gb_mode = MODE_CGB_32_COLOR;
                          break;

                case 'd': options.tile_dedupe_enabled = false;
                          break;
                case 'f': options.tile_dedupe_flips = false;
                          break;
                case 'p': options.tile_dedupe_palettes = false;
                          break;

                case 'v': log_set_level(OUTPUT_LEVEL_VERBOSE);
                          break;
                case 'e': log_set_level(OUTPUT_LEVEL_ONLY_ERRORS);
                          break;
                case 'q': log_set_level(OUTPUT_LEVEL_QUIET);
                          break;
            }
        } else {

            // Load output filename if specified
            if (filename_out[0] == '\0')
                strncpy(filename_out, argv[i], STR_FILENAME_MAX);
        }
    }

    // Apply the finalized options
    tilemap_options_set(&options);

// TODO: validate color count vs gb_mode
// TODO: update_filename()

    // If output filename wasn't specified, then
    // try to set it based on the input filename
    if (filename_out[0] == '\0') {

        copy_filename_without_extension(filename_noext, argv[ARG_INPUT_FILE]);

        switch (options.image_format) {
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

    // Process and export the image
    if (!tilemap_process_and_save_image(&src_image, &src_colors, &filename_out[0] )) {

        if (tilemap_error_get() != TILE_ID_OK) {
            log_error("%s\n", tilemap_error_get_string() );
        }
        return false;
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
            "  -p          Turn OFF Map tile deduplication of ALTERNATE PALLETE (.gbm only)\n"
            "\n"
            "  -q          Quiet, suppress all output\n"
            "  -e          Errors only, suppress all non-error output\n"
            "  -v          Verbose output during conversion\n"
            "\n"
            "Examples\n"
            "   png2gbtiles spritesheet.png -gbr spritesheet.gbr\n"
            "   png2gbtiles worldmap.png -gbm -d -f -p worldmap.gbm\n"
            "   png2gbtiles worldmap.png -gbm \n");
}