gimp tilemap generator and exporter plugin
===========

GIMP plugin for exporting tilemaps and tilesets from an image. Current use is for gameboy console game development.

Features
 * Tile deduplication

Tile Map export formats:
 * C Source (GBDK / ZGB)

Tile Set export formats:
 * C (GBDK / ZGB)
 * GBR (Game Boy Tile Designer ( http://www.devrs.com/gb/hmgd/gbtd.html )

Tile Set import formats:
 * GBR (Game Boy Tile Designer ( http://www.devrs.com/gb/hmgd/gbtd.html )

Supported formats:
 * GB 2bpp

OS binaries available for:
 * Linux
 * TODO: Windows


## Acknowledgement:


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
