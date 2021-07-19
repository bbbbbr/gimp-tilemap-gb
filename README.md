Tilemap GB (Gimp Plugin and command line tool): tilemap generator and exporter for Game Boy / Game Boy Color / GBDK & ZGB
===========

There are two versions of the tool:

## Standalone, command line version: [png2gbtiles](/console)

## GIMP plugin for importing & exporting Game Boy game tilemaps and tilesets.

You can create a level map in Gimp (or other image editors) then export it to either C Source files for the GBDK, or export as .gbr / .gbm for editing and further work in either GBTD or GBMB. You can also import from .gbr and .gbm files, and re-export to them (tile order may change and not all app prefs may survive the import/re-export trip).

(Also see Zal0's command line [converters from GBR/GBM -> .c/.h.](https://github.com/Zal0/ZGB/tree/master/tools)) 

*This plugin is functional, but not polished. Don't be surprised to find some rough edges or annoyances.

Download compiled GIMP Plugin executables here:
 * [Linux GIMP 2.8+](/bin/linux)
 * [Windows GIMP 2.10.12+](/bin/windows)

Download compiled Console executables here:
 * [Linux](/console/bin/linux)
 * [Windows](/console/bin/windows)

Or check [Releases](https://github.com/bbbbbr/gimp-tilemap-gb/releases)

![Opening a Game Boy Map Builder map as an image in GIMP](https://raw.githubusercontent.com/bbbbbr/gimp-tilemap/master/info/gimp-tilemap-plugin-screenshot.png)


Features (DMG & CGB)
 * Tile deduplication (same colors/same pattern)
 * Optional remapping of colors to a specific palette (user supplied palette or to repair per-tile color limit errors with the images current palette)

CGB only features
 * Flipped X/Y tile deduplication
 * Alternate Palette tile deduplication

Tile Map export formats:
* GBM (Game Boy Map Builder https://github.com/gbdk-2020/GBTD_GBMB )
* C Source (GBDK / ZGB)

Tile Set export formats:
 * C (GBDK / ZGB)
 * GBR (Game Boy Tile Designer https://github.com/gbdk-2020/GBTD_GBMB )

Tile Set & Map import formats:
 * GBR
 * GBM

Color Gameboy Support (CGB):
 * Import & Export

Supported formats:
 * GB 2bpp
 * CGB Color mode

Settings:
 * The plug-in will attempt to preserve Tile & Map Export settings across import and later export (they get attached to the native GIMP XCF image as a Parasite that persists accross save/load).
 * Option to ignore CGB per-tile palette violation errors. The tile will still be forced to 4 colors and it will use the first palette found in the tile.


## Usage / Cautions:
 * Work on an 8x8 grid. It will save you time, trouble and ROM size later on.
 * Available tile sizes are: 8x8, 8x16, 16x16, 32x32
 * The source image should be an even multiple of the tile size.
 * While C Source / GBM Maps can export to all tile sizes, working with 8x8 is sometimes most practical for hardware (unless set up for meta-tiles).
 * Exported GBR should have no more than 4 colors (DMG) or 32 colors (CGB)
 * In CGB mode the 32 color palette is broken up into 8 separate, consecutive sub-palettes of 4 colors each
 * When exporting in CGB mode, tiles *should not* use colors from more than one sub-palette at a time. Export will fail if they do, unless one of the following is enabled:
   * "Ignore CGB Palette Errors"
   * "Try to Repair Palette Errors" are enabled.
 * The only metadata preserved across import and later re-export for GBR / GBM files are the Tile/Map Export Settings. Other data such as recently used files, etc are not currently preserved when exporting back out. (Maybe later).
 * If you export using this path: GIMP -> GBTD or GBMB -> C source, then you'll need to set up the export options for GBTD (path, tiles to export, etc) and GBMB (path, location format, etc). Some, but not all, of them will be pre-populated.

## Quick instructions:

Native compile/install on Linux using below.

```
If GIMP & build tools not yet installed:
(example for debian/ubuntu/mint)
 * sudo apt install gimp
 * sudo apt install build-essential
 * sudo apt install libgimp2.0-dev

Then:
* cd gimp-rom-bin
* make

Then copy the resulting "file-tilemap" to your GIMP plugin folder, depends on version

Plug-in folder locations:
 Linux: ~/.gimp-2.8/plug-ins  , or ~/.config/GIMP/2.10/plug-ins
 Windows: C:\Program Files\GIMP 2\lib\gimp\2.0\plug-ins

```
Guide for [Cross-compiling to Windows on Linux](https://github.com/bbbbbr/gimp-rom-bin/blob/master/doc/GIMP%20jhbuild%20for%20Windows%20on%20Linux.md)


## Requirements:
* Image must be indexed color and not have more colors than output format supports, unless one of the following is enabled:
   * "Ignore CGB Palette Errors"
   * "Remap/Repair Palette Errors" and "Remap using a Palette File".

## Known limitations & Issues:
* GBMB Export only populates the Export Location Format if it was present on a preceding Import. Otherwise it must be done manually.
* When a GBMB Map is imported (along with matching GBTD Tileset), any tiles not used on the map will get discarded on re-export.
* Tile set ordering may change on export if the GBMB Map changes (tile set order is based on the when a given tiles is first encountered while scanning left -> right, top -> bottom)


