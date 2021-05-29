png2gbtiles: a standalone, command line tool for converting png tilesets and tilemaps to .gbr and .gbm (and possibly .c) files
===========

Builds available for Linux and Windows

See notes about Usage & Cautions in the main project's [Readme](https://github.com/bbbbbr/gimp-tilemap-gb/blob/master/README.md)

```
Usage
   png2gbtiles input_file.png -gbr|-gbm|-csource [options] [output_file]

Options

  -g          Force DMG color mode (4 colors or less only)
  -c          Force CGB color mode (up to 32 colors)

  -d          Turn OFF Map tile deduplication of tile PATTERN (.gbm only)
  -f          Turn OFF Map tile deduplication of FLIP X/Y (.gbm only)
  -p          Turn OFF Map tile deduplication of ALTERNATE PALETTE (.gbm only)

  -i          Ignore Palette Errors (CGB will use highest guessed palette #)

  -q          Quiet, suppress all output
  -e          Errors only, suppress all non-error output
  -v          Verbose output during conversion

Examples
   png2gbtiles spritesheet.png -gbr spritesheet.gbr
   png2gbtiles worldmap.png -gbm -d -f -p worldmap.gbm
   png2gbtiles worldmap.png -gbm 
```
