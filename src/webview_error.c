#include "zend_API.h"
#include "zend_exceptions.h"
#include "php_webview.h"

#include "webview_error.h"

WEBVIEW_API const char *webview_error_to_string(webview_error_t error)
{
    switch (error) {
        case WEBVIEW_ERROR_MISSING_DEPENDENCY:
            return "Missing dependency";
        case WEBVIEW_ERROR_CANCELED:
            return "Operation canceled";
        case WEBVIEW_ERROR_INVALID_STATE:
            return "Invalid state";
        case WEBVIEW_ERROR_INVALID_ARGUMENT:
            return "Invalid argument";
        case WEBVIEW_ERROR_UNSPECIFIED:
            return "Unspecified error";
        case WEBVIEW_ERROR_DUPLICATE:
            return "Duplicate";
        case WEBVIEW_ERROR_NOT_FOUND:
            return "Not found";
        default:
            return "Unknown error";
    }
}

WEBVIEW_API void php_webview_throw_exception(webview_error_t error, const char *message)
{
    char full_message[512];
    const char *error_name = webview_error_to_string(error);
    snprintf(full_message, sizeof(full_message), "[%s (%d)] %s", error_name, error, message ? message : "");
    zend_throw_exception_ex(webview_exception_ce, error, "%s", full_message);
}

