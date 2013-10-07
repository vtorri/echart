
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [2])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], [m4_eval(v_maj + v_min)])
m4_define([lt_rev], [v_mic])
m4_define([lt_age], [v_min])

AC_INIT([echart], [v_ver], [vincent.torri.remove@gmail.com])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 subdir-objects dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT([win32-dll disable-static])

version_info="lt_cur:lt_rev:lt_age"
release_info="v_rel"
AC_SUBST([version_info])
AC_SUBST([release_info])

VMAJ=v_maj
AC_SUBST([VMAJ])


### Additional options to configure


### Default options with respect to host

AC_CANONICAL_HOST

requirements_echart_libs=""
AC_SUBST([requirements_echart_libs])

efl_version="1.7.0"

requirements_echart_pc="enesim >= 0.0.20 eina >= ${efl_version}"
AC_SUBST([requirements_echart_pc])

requirements_echart_bin_pc="enesim >= 0.0.20 ecore-evas >= ${efl_version} ecore >= ${efl_version} evas >= ${efl_version} eina >= ${efl_version}"


### Checks for programs

AC_PROG_CC

PKG_PROG_PKG_CONFIG


### Checks for libraries

PKG_CHECK_MODULES([ECHART], [${requirements_echart_pc}])
PKG_CHECK_MODULES([ECHART_BIN], [${requirements_echart_bin_pc}])


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O

EFL_CHECK_C_COMPILER_FLAGS([ECHART_LIB], [-Wall -Wextra -Wshadow -Wdeclaration-after-statement -Wmissing-prototypes -Wstrict-prototypes -Wpointer-arith -Wno-missing-field-initializers -fvisibility=hidden -fdata-sections -ffunction-sections])

EFL_CHECK_C_COMPILER_FLAGS([ECHART_BIN], [-Wall -Wextra -Wshadow -Wdeclaration-after-statement -Wmissing-prototypes -Wstrict-prototypes -Wpointer-arith -Wno-missing-field-initializers -fvisibility=hidden -fdata-sections -ffunction-sections])


### Checks for linker characteristics


### Checks for library functions

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
echo "Configuration Options Summary:"
echo
echo "  OS...................: ${host_os}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo

