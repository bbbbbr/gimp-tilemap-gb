## Process this file with automake to produce Makefile.in

libgimpui = $(top_builddir)/libgimp/libgimpui-$(GIMP_API_VERSION).la
libgimpconfig = $(top_builddir)/libgimpconfig/libgimpconfig-$(GIMP_API_VERSION).la
libgimpwidgets = $(top_builddir)/libgimpwidgets/libgimpwidgets-$(GIMP_API_VERSION).la
libgimp = $(top_builddir)/libgimp/libgimp-$(GIMP_API_VERSION).la
libgimpcolor = $(top_builddir)/libgimpcolor/libgimpcolor-$(GIMP_API_VERSION).la
libgimpbase = $(top_builddir)/libgimpbase/libgimpbase-$(GIMP_API_VERSION).la
libgimpmath = $(top_builddir)/libgimpmath/libgimpmath-$(GIMP_API_VERSION).la

if OS_WIN32
mwindows = -mwindows
endif

if HAVE_WINDRES
include $(top_srcdir)/build/windows/gimprc-plug-ins.rule
file_tilemap_export_RC = file_tilemap_export.rc.o
endif

AM_LDFLAGS = $(mwindows)

libexecdir = $(gimpplugindir)/plug-ins

libexec_PROGRAMS = file_tilemap_export

file_tilemap_export_SOURCES = \
	file-tilemap.c \
	format_snesgb_2bpp.c \
	lib_gbm.c \
	lib_gbm_export.c \
	lib_gbm_file_utils.c \
	lib_gbm_import.c \
	lib_gbm_ops.c \
	lib_gbr.c \
	lib_gbr_export.c \
	lib_gbr_file_utils.c \
	lib_gbr_import.c \
	lib_gbr_ops.c \
	lib_rom_bin.c \
	lib_tilemap.c \
	rom_utils.c \
	tilemap_format_gbdk_c_source.c \
	tilemap_io.c \
	tilemap_path_ops.c \
	tilemap_read.c \
	tilemap_tiles.c \
	tilemap_write.c \
	export-dialog.c \
	tilemap_error.c \
	hash.c


INCLUDES = \
	-I$(top_srcdir)	\
	$(GTK_CFLAGS)	\
	-I$(includedir)

LDADD = \
	$(libgimpui)		\
	$(libgimpwidgets)	\
	$(libgimpconfig)	\
	$(libgimp)		\
	$(libgimpcolor)		\
	$(libgimpmath)		\
	$(libgimpbase)		\
	$(GTK_LIBS)		\
	$(RT_LIBS)		\
	$(INTLLIBS)		\
	$(file_tilemap_export_RC)
