# CC = gcc
# DEL = rm -f
# SRCDIR = src
# BUILDDIR = build
# OUTDIR = bin
# LODEPNGDIR = lodepng
# TILEMAPGBDIR = tilemap_gb
# INCS = -I"$(SRCDIR)" -I"$(LODEPNGDIR)" -I"$(TILEMAPGBDIR)"
# OBJS = $(BUILDDIR)/lodepng.o $(BUILDDIR)/pngb.o $(BUILDDIR)/main.o
# EXE = pngb
# CFLAGS = $(INCS)
# LFLAGS = -s

# $(EXE):	$(BUILDDIR) $(OBJS)
# 	$(CC) $(CFLAGS) -Wl,$(LFLAGS) -o $(OUTDIR)/$(EXE) $(OBJS)

# $(BUILDDIR) :
# 	mkdir $(BUILDDIR)

# $(BUILDDIR)/lodepng.o: $(LODEPNGDIR)/lodepng.c
# 	$(CC) $(CFLAGS) -c $(LODEPNGDIR)/lodepng.c -o $(BUILDDIR)/lodepng.o

# $(BUILDDIR)/pngb.o: $(SRCDIR)/pngb.c
# 	$(CC) $(CFLAGS) -c $(SRCDIR)/pngb.c -o $(BUILDDIR)/pngb.o

# $(BUILDDIR)/main.o: $(SRCDIR)/main.c
# 	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c -o $(BUILDDIR)/main.o


# clean:
# 	$(DEL) $(BUILDDIR)/*.o
# 	$(DEL) $(OUTDIR)/$(EXE)

CC = gcc
DEL = rm -f
SRCDIR = src
LODEPNGDIR = lodepng
TILEMAPGBDIR = ../src
OUTDIR = bin
INCS = -I"$(SRCDIR)" -I"$(LODEPNGDIR)" -I"$(TILEMAPGBDIR)"
CFLAGS = $(INCS)

# Add all c source files
CALL = $(wildcard src/*.c) \
		$(wildcard lodepng/*.c) \
		$(wildcard ../src/*.c)
# Remove some files that won't/can't be used
CEXCLUDE = $(TILEMAPGBDIR)/tilemap_write.c $(TILEMAPGBDIR)/tilemap_read.c $(TILEMAPGBDIR)/file-tilemap.c $(TILEMAPGBDIR)/export-dialog.c
CFILES = $(filter-out $(CEXCLUDE),$(CALL))
COBJ = $(CFILES:.c=.o)


LDFLAGS = -s

png2gbtiles: $(COBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$@ $^ $(LDFLAGS)


clean:
	rm -f $(COBJ) $(OUTDIR)/png2gbtiles