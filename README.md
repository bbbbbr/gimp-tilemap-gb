gimp tilemap generator and exporter plugin
===========

GIMP plugin for exporting tilemaps and tilesets from an image. Current use is for gameboy console game development.

You can create a level map in Gimp (or other image editors) then export it to either C Source files for the GBDK, or export as .gbr / .gbm for editing and further work in either GBTD or GBMB. You can also import from .gbr and .gbm files, and re-export to them (tile order may change and not all app prefs may survive the import/re-export trip though).

*This plugin is a work in progress and has lots of rough edges right now! You've been warned :)*

Features
 * Tile deduplication

Tile Map export formats:
 * C Source (GBDK / ZGB)

Tile Set export formats:
 * C (GBDK / ZGB)
 * GBR (Game Boy Tile Designer http://www.devrs.com/gb/hmgd/gbtd.html )
 * GBM (Game Boy Map Builder http://www.devrs.com/gb/hmgd/gbmb.html )

Tile Set import formats:
 * GBR
 * GBM

Supported formats:
 * GB 2bpp

OS binaries available for:
 * Linux
 * TODO: Windows


## Usage:
 * Exported C Source / GBM Map Tilesets *always* use a tile size of 8x8 pixels, and the source image should be an even multiple of the tile size
 * Exported GBR tilesets should have an image width of 8, 16 or 32 pixels. Tile size width and height will auto set to be the same as the image width. This means 8x16 is not currently supported. (Maybe a dialog selecting export size later).
 * Exported GBR should have no more than 4 colors, for now.
 * Metadata stored in the GBR / GBM files, such as recently used files, etc is not currently preserved when exporting back out. (Maybe coming later, it's do-able and partially in there).


## Quick instructions:

Native compile/install on Linux using below.

```
gimptool-2.0 --install file-tilemap-export.c
    or
 make (and then copy to your GIMP plugin folder, depends on version)

Plug-in folder locations:
 Linux: ~/.gimp-2.8/plug-ins
 Windows: C:\Program Files\GIMP 2\lib\gimp\2.0\plug-ins

```

## Requirements:
* Image must be indexed color and not have more colors than output format supports

## Known limitations & Issues:

## GIMP usage hints:
