/*
	+----------------------------------------------------------------------+
	| PHP Version 7                                                        |
	+----------------------------------------------------------------------+
	| Copyright (c) 1997-2013 The PHP Group                                |
	+----------------------------------------------------------------------+
	| This source file is subject to version 3.01 of the PHP license,      |
	| that is bundled with this package in the file LICENSE, and is        |
	| available through the world-wide-web at the following url:           |
	| http://www.php.net/license/3_01.txt                                  |
	| If you did not receive a copy of the PHP license and are unable to   |
	| obtain it through the world-wide-web, please send a note to          |
	| license@php.net so we can mail you a copy immediately.               |
	+----------------------------------------------------------------------+
	| Author:                                                              |
	+----------------------------------------------------------------------+
*/

//php5 -> php7
//demo ZEND_HASH_FOREACH_KEY_VAL https://github.com/Neeke/SeasLog/blob/391591210c24ce3d5a05523d4cabce00522b1eb8/seaslog.c
//doc https://wiki.php.net/phpng-upgrading
//article http://www.sklar.com/software/php/2015/03/23/converting-a-php-extension-to-php-7/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_array_to_dom.h"
#include "ext/libxml/php_libxml.h"

//add support php5.3.6. Details https://github.com/php/php-src/commit/054e1cafa0f7d83bf9318431cc0ea54a5ff6afaf
#define ZEND_FE_END     { NULL, NULL, NULL, 0, 0 }
#define PHP_FE_END      ZEND_FE_END

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_to_dom, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, doc, DOMDocument, 0)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, root_element)
ZEND_END_ARG_INFO();

/* If you declare any globals in php_array_to_dom.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(array_to_dom)
*/

/* True global resources - no need for thread safety here */
static int le_array_to_dom;

/* {{{ array_to_dom_functions[]
 *
 * Every user visible function must have an entry in array_to_dom_functions[].
 */
const zend_function_entry array_to_dom_functions[] = {
	PHP_FE(array_to_dom,	arginfo_array_to_dom)
	PHP_FE_END	/* Must be the last line in array_to_dom_functions[] */
};
/* }}} */

/* {{{ array_to_dom_module_entry
 */
zend_module_entry array_to_dom_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"array_to_dom",
	array_to_dom_functions,
	PHP_MINIT(array_to_dom),
	PHP_MSHUTDOWN(array_to_dom),
	PHP_RINIT(array_to_dom),	/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(array_to_dom),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(array_to_dom),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_ARRAY_TO_DOM_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ARRAY_TO_DOM
ZEND_GET_MODULE(array_to_dom)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("array_to_dom.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_array_to_dom_globals, array_to_dom_globals)
	STD_PHP_INI_ENTRY("array_to_dom.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_array_to_dom_globals, array_to_dom_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_array_to_dom_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_array_to_dom_init_globals(zend_array_to_dom_globals *array_to_dom_globals)
{
	array_to_dom_globals->global_value = 0;
	array_to_dom_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(array_to_dom)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(array_to_dom)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(array_to_dom)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(array_to_dom)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(array_to_dom)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "array_to_dom support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

static int php_is_valid_tag_name(char *s)
{
	if (isalpha(*s) || *s == '_') {
		++s;
		while (*s) {
			if (!isalnum(*s) && *s != '_' && *s != '-' && *s != '.') return 0;
			++s;
		}
		return 1;
	}
	return 0;
}

static void php_array_to_dom(xmlNodePtr node, zval *val);

static void php_array_to_dom_array(xmlNodePtr node, zval *val) /* {{{ */
{
	HashTable *ht;
	double dbl;
	int len;

	ZVAL_DEREF(val);
	if (Z_TYPE_P(val) == IS_ARRAY) {
		ht = HASH_OF(val);
	} else {
		ht = Z_OBJPROP_P(val);
	}

	if (ht == NULL) {
		return;
	}

	if (ht->u.v.nApplyCount > 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "recursion detected");
		return;
	}

	if (zend_hash_num_elements(ht) > 0) {
		char *tag, buf[128], *cur_val;
		zval *data;
		zend_ulong num_key;
		zend_string *str_key;

		xmlNodePtr e;

		ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, data) {
			HashTable *tmp_ht = HASH_OF(data);
			if (tmp_ht) tmp_ht->u.v.nApplyCount++;

			if (str_key) {//HASH_KEY_IS_STRING
				if (ZSTR_VAL(str_key)[0] == '\0' && Z_TYPE_P(val) == IS_OBJECT) {
					// Skip protected and private members.
					if (tmp_ht) {
						tmp_ht->u.v.nApplyCount--;
					}
					continue;
				}

				//Begin get current value
				cur_val=NULL;
				ZVAL_DEREF(data);
				switch (Z_TYPE_P(data)) {
					case IS_TRUE:
						cur_val = BAD_CAST "1";
						len = 1;
					break;
					case IS_FALSE:
						// Nothing - "false" is an empty string, result is an empty node.
						cur_val=BAD_CAST "";
						len = 0;
					break;

					case IS_LONG:
						len = sprintf(buf, "%ld", Z_LVAL_P(data));
						cur_val = BAD_CAST buf;
					break;

					case IS_DOUBLE:
						dbl = Z_DVAL_P(data);
						if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
							len = snprintf(buf, sizeof(buf), "%.*k", (int)EG(precision), dbl);
						} else {
							len = sprintf(buf, "%f", dbl);
						}
						cur_val = BAD_CAST buf;
					break;

					case IS_STRING:
						cur_val = BAD_CAST Z_STRVAL_P(data);
						len = Z_STRLEN_P(data);
					break;

					case IS_ARRAY:
					case IS_OBJECT:
					case IS_NULL:
					break;

					default:
					break;
				}
				//End get current value


				//if str_key = "*" and node name = "item", set new node name
				if ( strcmp(ZSTR_VAL(str_key), "*")==0 ){
					//xmlNodeSetName(node, BAD_CAST Z_STRVAL_P(data));
					if (cur_val!=NULL && strcmp(node->name, "item")==0) xmlNodeSetName(node, cur_val);

					if (tmp_ht) {
						tmp_ht->u.v.nApplyCount--;
					}
					continue;
				}

				//if str_key = ".", set content
				if ( strcmp(ZSTR_VAL(str_key), ".")==0 ){
					if (cur_val!=NULL) xmlNodeSetContentLen(node, cur_val, len);

					if (tmp_ht) {
						tmp_ht->u.v.nApplyCount--;
					}
					continue;
				}

				//if first symbol of str_key is "@", it's attribute
				if ( ZSTR_VAL(str_key)[0] == '@' ){
					if (cur_val!=NULL && strlen(ZSTR_VAL(str_key))>1 ) xmlNewProp(node, BAD_CAST ZSTR_VAL(str_key) + 1, cur_val);

					if (tmp_ht) {
						tmp_ht->u.v.nApplyCount--;
					}
					continue;
				}

				if (php_is_valid_tag_name(ZSTR_VAL(str_key))) tag = ZSTR_VAL(str_key);
				else tag = "item";
			}
			else {
				//key is number
				tag = "item";
			}

			//xmlNewChild(xmlNodePtr parent, xmlNsPtr ns, const xmlChar *name, const xmlChar *content);
			e = xmlNewChild(node, NULL, BAD_CAST tag, NULL);
			php_array_to_dom(e, data);

			if (tmp_ht) {
				tmp_ht->u.v.nApplyCount--;
			}
		} ZEND_HASH_FOREACH_END();
	}
}

static void php_array_to_dom(xmlNodePtr node, zval *val) /* {{{ */
{
	xmlNodePtr text = NULL;
	int len;
	char buf[128];
	double dbl;

	ZVAL_DEREF(val);
	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			text = xmlNewTextLen(BAD_CAST "", 0);
		break;

		case IS_TRUE:
			text = xmlNewTextLen(BAD_CAST "1", 1);
		break;
		case IS_FALSE:
			// Nothing - "false" is an empty string, result is an empty node.
			text = xmlNewTextLen(BAD_CAST "", 0);
		break;

		case IS_LONG:
			len = sprintf(buf, "%ld", Z_LVAL_P(val));
			text = xmlNewTextLen(BAD_CAST buf, len);
		break;

		case IS_DOUBLE:
			dbl = Z_DVAL_P(val);
			if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
				len = snprintf(buf, sizeof(buf), "%.*k", (int)EG(precision), dbl);
			} else {
				len = sprintf(buf, "%f", dbl);
			}
			text = xmlNewTextLen(BAD_CAST buf, len);
		break;

		case IS_STRING:
			text = xmlNewTextLen(BAD_CAST Z_STRVAL_P(val), Z_STRLEN_P(val));
		break;

		case IS_ARRAY:
		case IS_OBJECT:
			php_array_to_dom_array(node, val);
		break;

		default:
			text = xmlNewTextLen(BAD_CAST "defa", 4);
		break;
	}

	if (text != NULL) {
		xmlAddChild(node, text);
	}
}
/* }}} */


/* {{{ proto string array_to_dom(mixed doc, mixed var [, string root_element])
	*/
PHP_FUNCTION(array_to_dom)
{
	zval *id, *var;
	zend_string *root_element_name;
	xmlNodePtr nodep = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, old_root_node;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oz|S", &id, &var, &root_element_name) == FAILURE) {
		return;
	}

	if (!root_element_name || !php_is_valid_tag_name(root_element_name->val)) {
		root_element_name = zend_string_init("root", sizeof("root")-1, 0);
	}

	nodep = php_libxml_import_node(id TSRMLS_CC);

	if (nodep) {
		doc = nodep->doc;
	}
	if (doc == NULL) {
		php_error(E_WARNING, "Invalid Document");
		RETURN_FALSE;
	}

	root_node = xmlNewNode(NULL, BAD_CAST root_element_name->val);
	old_root_node = xmlDocSetRootElement(doc, root_node);
	if (old_root_node != NULL) {
		xmlUnlinkNode(old_root_node);
		xmlFreeNode(old_root_node);
	}

	php_array_to_dom(root_node, var);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
