#ifndef PHP_PHP_WEBVIEW_ERROR_H
#define PHP_PHP_WEBVIEW_ERROR_H

#include "webview/webview.h"

#if defined(__cplusplus)
extern "C" {
#endif

WEBVIEW_API const char *webview_error_to_string(webview_error_t error);
WEBVIEW_API void php_webview_throw_exception(webview_error_t error, const char *message);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* PHP_PHP_WEBVIEW_ERROR_H */
