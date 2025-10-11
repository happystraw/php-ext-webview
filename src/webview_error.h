#ifndef PHP_WEBVIEW_ERROR_H
#define PHP_WEBVIEW_ERROR_H

#include "webview/webview.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include "zend_API.h"

WEBVIEW_API const char *webview_error_to_string(webview_error_t error);
WEBVIEW_API void php_webview_throw_exception(zend_class_entry *exception_ce, webview_error_t error, const char *message);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* PHP_WEBVIEW_ERROR_H */
