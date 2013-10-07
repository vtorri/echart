
bin_PROGRAMS = src/bin/echart

src_bin_echart_SOURCES = \
src/bin/echart.c

src_bin_echart_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ECHART_BIN_CFLAGS@

src_bin_echart_LDADD = \
src/lib/libechart.la \
@ECHART_BIN_LIBS@
