
lib_LTLIBRARIES = src/lib/libechart.la

includes_HEADERS = src/lib/Echart.h
includesdir = $(pkgincludedir)-@VMAJ@

src_lib_libechart_la_SOURCES = \
src/lib/echart_chart.c \
src/lib/echart_column.c \
src/lib/echart_data.c \
src/lib/echart_line.c \
src/lib/echart_main.c \
src/lib/echart_private.h

src_lib_libechart_la_CPPFLAGS = \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
-DECHART_BUILD \
@ECHART_CFLAGS@

src_lib_libechart_la_CFLAGS = \
@ECHART_LIB_CFLAGS@

src_lib_libechart_la_LIBADD = @ECHART_LIBS@

src_lib_libechart_la_LDFLAGS = -no-undefined -version-info @version_info@ @release_info@
