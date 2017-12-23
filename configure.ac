AC_INIT([zidl], [2.0.0])
AC_CONFIG_AUX_DIR([src])
AM_INIT_AUTOMAKE

# the lemon outputs are named *.c by default, but contains c++ code
# this seems to work when invoked before LT_INIT
AC_PROG_CXX
AC_PROG_CC(["$CXX"])

AC_CHECK_PROG(has_re2c, "re2c", "yes", "no")
if test "$has_re2c" == "no"
then
	AC_MSG_ERROR([re2c not installed])
fi

AC_CHECK_PROG(has_lemon, "lemon", "yes", "no")
if test "$has_lemon" == "no"
then
	AC_MSG_ERROR([lemon not installed])
fi

AC_CONFIG_FILES([Makefile] [src/Makefile])
#AC_CONFIG_SUBDIRS([src])
AC_OUTPUT
