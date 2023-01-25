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

#include "console_import_imagefile.h"
#include "console_export_imagefile.h"

#include "tilemap_path_ops.h"

#include "image_remap.h"


#define ARG_INPUT_FILE    1
#define ARG_OUTPUT_MODE   2
#define ARG_OPTIONS_START 3

const char * const opt_gbr = "-gbr";
const char * const opt_gbm = "-gbm";
const char * const opt_csource   = "-csource";
const char * const opt_png_out   = "-png";
const char * const opt_repair_pal  = "-repairpal";

const char * const opt_varname     = "-var=";
const char * const opt_remap_pal   = "-pal=";
const char * const opt_bank        = "-bank=";
const char * const opt_tile_size   = "-tilesz=";
const char * const opt_tile_palette_size   = "-tilepalsz=";
const char * const opt_tileid_offset   = "-tileorg=";

// user overrides for default settings
tile_process_options user_options;
char filename_in[STR_FILENAME_MAX] = {'\0'};
char filename_out[STR_FILENAME_MAX] = {'\0'};

int convert_image_from_png(void);
void apply_user_options(tile_process_options *);;
void clear_user_options(void);
int handle_args(int, char * []);
void display_help(void);



int main( int argc, char *argv[] )  {

    if (handle_args(argc, argv)) {

        if (user_options.image_format == FORMAT_PNG_OUT) {

            if (console_tileformat_to_image_file(filename_in, filename_out))
                return 0; // Exit with Success
        } else {

            if (console_image_to_tileformat_file(&user_options, filename_in, filename_out))
                return 0; // Exit with Success
        }
    }

    return 1; // Exit with failure
}


int handle_args( int argc, char * argv[] ) {

    int i;
    char filename_noext[STR_FILENAME_MAX] = {'\0'};

    options_reset(&user_options);

    if( argc < 3 ) {
        log_error("Error: At least two arguments are required\n\n");
        display_help();
        return false;
    }

    // Copy input filename
    strncpy(filename_in, argv[ARG_INPUT_FILE], STR_FILENAME_MAX);

    // Select output mode (from second argument)
    if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_gbr, strlen(opt_gbr))) {
        user_options.image_format = FORMAT_GBR;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_gbm, strlen(opt_gbr))) {
        user_options.image_format = FORMAT_GBM;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_csource, strlen(opt_csource))) {
        user_options.image_format = FORMAT_GBDK_C_SOURCE;

    } else if (0 == strncmp(argv[ARG_OUTPUT_MODE], opt_png_out, strlen(opt_png_out))) {
        user_options.image_format = FORMAT_PNG_OUT;

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
            if (0 == strncmp(argv[i], opt_remap_pal, strlen(opt_remap_pal))) {
                // Extract filename for user supplied palette
                snprintf(user_options.remap_pal_file, STR_FILENAME_MAX, "%s", argv[i] + strlen(opt_remap_pal));
                user_options.remap_pal = true;
            } else if (0 == strncmp(argv[i], opt_repair_pal, strlen(opt_repair_pal))) {
                // Repair palette mode: Remap the palette, but use the images own palette
                user_options.remap_pal = true;
            }
            else if (0 == strncmp(argv[i], opt_varname, strlen(opt_varname))) {
                snprintf(user_options.varname, STR_FILENAME_MAX, "%s", argv[i] + strlen(opt_varname));
            }
            else if (0 == strncmp(argv[i], opt_bank, strlen(opt_bank))) {
                user_options.bank_num = strtol(argv[i] + strlen(opt_bank), NULL, 0);
            }
            else if (0 == strncmp(argv[i], opt_tileid_offset, strlen(opt_tileid_offset))) {
                user_options.map_tileid_offset = strtol(argv[i] + strlen(opt_tileid_offset), NULL, 0);
            }
            else if (0 == strncmp(argv[i], opt_tile_size, strlen(opt_tile_size))) {
                if (0 == strncmp(argv[i] + strlen(opt_tile_size), "8x8", strlen("8x8"))) {
                    user_options.tile_width = 8;
                    user_options.tile_height = 8;
                }
                else if (0 == strncmp(argv[i] + strlen(opt_tile_size), "8x16", strlen("8x16"))) {
                    user_options.tile_width = 8;
                    user_options.tile_height = 16;
                }
                else if (0 == strncmp(argv[i] + strlen(opt_tile_size), "16x16", strlen("16x16"))) {
                    user_options.tile_width = 16;
                    user_options.tile_height = 16;
                }
                else if (0 == strncmp(argv[i] + strlen(opt_tile_size), "32x32", strlen("32x32"))) {
                    user_options.tile_width = 32;
                    user_options.tile_height = 32;
                } else {
                    log_error("Error: Invalid tile size: %s\n\n", argv[i] + strlen(opt_tile_size));
                    display_help();
                    return false;
                }
            }
            else if (0 == strncmp(argv[i], opt_tile_palette_size, strlen(opt_tile_palette_size))) {
                if (0 == strncmp(argv[i] + strlen(opt_tile_palette_size), "8x8", strlen("8x8"))) {
                    user_options.palette_tile_width = 8;
                    user_options.palette_tile_height = 8;
                }
                else if (0 == strncmp(argv[i] + strlen(opt_tile_palette_size), "8x16", strlen("8x16"))) {
                    user_options.palette_tile_width = 8;
                    user_options.palette_tile_height = 16;
                }
                else if (0 == strncmp(argv[i] + strlen(opt_tile_palette_size), "16x16", strlen("16x16"))) {
                    user_options.palette_tile_width = 16;
                    user_options.palette_tile_height = 16;
                }
                else if (0 == strncmp(argv[i] + strlen(opt_tile_palette_size), "32x32", strlen("32x32"))) {
                    user_options.palette_tile_width = 32;
                    user_options.palette_tile_height = 32;
                } else {
                    log_error("Error: Invalid palette tile size: %s\n\n", argv[i] + strlen(opt_tile_palette_size));
                    display_help();
                    return false;
                }

                printf("Tile width and height (%d x %d) \n", user_options.palette_tile_width, user_options.palette_tile_height);
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
        log_verbose("Filename out is empty\n");

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

            case FORMAT_PNG_OUT:
                snprintf(&filename_out[0], STR_FILENAME_MAX, "%s%s",  &filename_noext[0], ".png");
                log_verbose("Filename out is now: %s\n", filename_out);
                break;
        }
    }

    // If user variable name wasn't specified then use file name with no path and extension isntead
    if (user_options.varname[0] == '\0')
        copy_filename_without_path_and_extension(user_options.varname, filename_out);

    return true;
}


void display_help(void) {

    log_standard("Usage\n"
            "   png2gbtiles input_file.png -gbr|-gbm|-csource [options] [output_file]\n"
            "\n"
            "   png2gbtiles input_file.gbm|.gbr -png [output_file]\n"
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
            "  -repairpal  Repair Palette Errors (Remap tile to best fit palette)\n"
            "  -pal=[file] Remap png to palette (pngs allowed: index and 24/32 bit RGB)\n"
            "\n"
            "  -var=[name]    Base name to use for export variables (otherwise filename)\n"
            "  -bank=[num]    Set bank number for all output modes\n"
            "  -tileorg=[num] Tile index offset for maps (instead of zero)\n"
            "  -tilesz=[size] Tile size (8x8, 8x16, 16x16, 32x32) \n"
            "  -tilepalsz=[size] Tile Palette size (8x8, 8x16, 16x16, 32x32) \n"
            "\n"
            "  -q          Quiet, suppress all output\n"
            "  -e          Errors only, suppress all non-error output\n"
            "  -v          Verbose output during conversion\n"
            "\n"
            "png -> gbr/gbm Examples\n"
            "   png2gbtiles spritesheet.png -gbr spritesheet.gbr\n"
            "   png2gbtiles worldmap.png -gbm -d -f -p worldmap.gbm\n"
            "   png2gbtiles worldmap.png -gbm \n"
            "   png2gbtiles worldmap.png -gbm -c -pal=mypal.pal -bank=4 -tileorg=64\n"
            "Remap Palette format: RGB in hex text, 1 color per line (ex: FF0080)\n"
            "\n"
            "gbr/gbm -> Example\n"
            "   png2gbtiles worlmap.gbm -png worlmap.png\n"
            );
}