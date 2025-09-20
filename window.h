#ifndef WEBVIEW_WINDOW_H
#define WEBVIEW_WINDOW_H

#include "webview/webview.h"

#if defined(__cplusplus)
extern "C" {
#endif

WEBVIEW_API webview_error_t webview_window_maximize(webview_t w);
WEBVIEW_API webview_error_t webview_window_minimize(webview_t w);
WEBVIEW_API webview_error_t webview_window_restore(webview_t w);
WEBVIEW_API webview_error_t webview_window_fullscreen(webview_t w);
WEBVIEW_API webview_error_t webview_window_unfullscreen(webview_t w);
WEBVIEW_API webview_error_t webview_window_hide(webview_t w);
WEBVIEW_API webview_error_t webview_window_show(webview_t w);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif // WEBVIEW_WINDOW_H
