/* webview extension for PHP */

#ifndef PHP_WEBVIEW_H
#define PHP_WEBVIEW_H

extern zend_module_entry webview_module_entry;
#define phpext_webview_ptr &webview_module_entry

#define PHP_WEBVIEW_VERSION "0.1.0"

// Include webview library header
#define WEBVIEW_STATIC "1"
#include "webview/webview.h"

// Webview object structure
typedef struct _php_webview_obj {
    webview_t webview;
    HashTable *bindings; // Store PHP callbacks for JS bindings
    zend_object std;
} php_webview_obj;

// Class entries
extern zend_class_entry *webview_hint_ce;
extern zend_class_entry *webview_ce;
extern zend_class_entry *webview_exception_ce;

// Helper macros
#define Z_WEBVIEW_OBJ_P(zv) php_webview_from_obj(Z_OBJ_P(zv))

static inline php_webview_obj *php_webview_from_obj(zend_object *obj) {
    return (php_webview_obj*)((char*)(obj) - XtOffsetOf(php_webview_obj, std));
}

#ifdef ZTS
#include "TSRM.h"
#endif

#if defined(ZTS) && defined(COMPILE_DL_WEBVIEW)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* PHP_WEBVIEW_H */
