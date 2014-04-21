dnl $Id$
dnl config.m4 for extension array_to_dom

PHP_ARG_ENABLE(array_to_dom, whether to enable array_to_dom support,
[  --enable-array_to_dom           Enable array_to_dom support])

if test -z "$PHP_LIBXML_DIR"; then
	PHP_ARG_WITH(libxml-dir, libxml2 install dir,
	[  --with-libxml-dir=DIR   array_to_dom: libxml2 install prefix], no, no)
fi

if test "$PHP_ARRAY_TO_DOM" != "no"; then

	if test "$PHP_LIBXML" = "no"; then
		AC_MSG_ERROR([array_to_dom extension requires LIBXML extension, add --enable-libxml])
	fi

	if test "$PHP_DOM" = "no"; then
		AC_MSG_ERROR([array_to_dom extension requires DOM extension, add --enable-dom])
	fi

	PHP_SETUP_LIBXML(XMLDIFF_SHARED_LIBADD, [
		PHP_NEW_EXTENSION(array_to_dom, array_to_dom.c, $ext_shared)
		PHP_ADD_EXTENSION_DEP(array_to_dom, dom, true)
		PHP_ADD_EXTENSION_DEP(array_to_dom, libxml, true)
		PHP_INSTALL_HEADERS([ext/array_to_dom/php_array_to_dom.h])
	], [
		AC_MSG_ERROR([xml2-config not found. Please check your libxml2 installation.])
	])

fi
