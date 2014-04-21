dnl $Id$
dnl config.m4 for extension array_to_dom

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(array_to_dom, for array_to_dom support,
dnl Make sure that the comment is aligned:
dnl [  --with-array_to_dom             Include array_to_dom support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(array_to_dom, whether to enable array_to_dom support,
dnl Make sure that the comment is aligned:
[  --enable-array_to_dom           Enable array_to_dom support])

if test "$PHP_ARRAY_TO_DOM" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-array_to_dom -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/array_to_dom.h"  # you most likely want to change this
  dnl if test -r $PHP_ARRAY_TO_DOM/$SEARCH_FOR; then # path given as parameter
  dnl   ARRAY_TO_DOM_DIR=$PHP_ARRAY_TO_DOM
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for array_to_dom files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ARRAY_TO_DOM_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ARRAY_TO_DOM_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the array_to_dom distribution])
  dnl fi

  dnl # --with-array_to_dom -> add include path
  dnl PHP_ADD_INCLUDE($ARRAY_TO_DOM_DIR/include)

  dnl # --with-array_to_dom -> check for lib and symbol presence
  dnl LIBNAME=array_to_dom # you may want to change this
  dnl LIBSYMBOL=array_to_dom # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ARRAY_TO_DOM_DIR/lib, ARRAY_TO_DOM_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ARRAY_TO_DOMLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong array_to_dom lib version or lib not found])
  dnl ],[
  dnl   -L$ARRAY_TO_DOM_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ARRAY_TO_DOM_SHARED_LIBADD)

  PHP_NEW_EXTENSION(array_to_dom, array_to_dom.c, $ext_shared)
fi
