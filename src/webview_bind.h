#ifndef PHP_WEBVIEW_BIND_H
#define PHP_WEBVIEW_BIND_H

#include "webview/webview.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include "zend_API.h"

typedef struct {
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
} php_webview_callback_t;

typedef enum {
    PHP_WEBVIEW_BIND_MODE_AUTO = 0,
    PHP_WEBVIEW_BIND_MODE_MANUAL = 1,
} php_webview_bind_mode_t;

// Structure to hold callback context including webview instance
typedef struct {
    php_webview_callback_t *binding;
    webview_t webview;
    zend_long bind_mode;
} php_webview_binding_context_t;

// Structure to hold dispatch callback context
typedef struct {
    php_webview_callback_t *fn;
    webview_t webview;
} php_webview_dispatch_context_t;

WEBVIEW_API webview_error_t php_webview_return_zend_string(webview_t w, const char *id, int status, zend_string *str, int json_encode);
WEBVIEW_API webview_error_t php_webview_return_exception(webview_t w, const char *id, int status, zend_object *exception);
WEBVIEW_API void php_webview_callback_addref(php_webview_callback_t *callback);
WEBVIEW_API void php_webview_callback_release(php_webview_callback_t *callback);
WEBVIEW_API void php_webview_binding_context_release(php_webview_binding_context_t *context);
WEBVIEW_API void php_webview_binding_callback(const char *id, const char *req, void *arg);
WEBVIEW_API void php_webview_dispatch_callback(webview_t w, void *arg);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* PHP_WEBVIEW_BIND_H */
