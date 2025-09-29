#include "php.h"
#include "ext/json/php_json.h"

#include "zend_smart_str.h"
#include "zend_exceptions.h"

#include "webview_error.h"
#include "webview_bind.h"

WEBVIEW_API webview_error_t php_webview_return_zend_string(webview_t w, const char *id, int status, zend_string *str, int json_encode)
{
    if (json_encode != 1) {
        return webview_return(w, id, status, ZSTR_VAL(str));
    }

    // json encode the string
    webview_error_t result;
    smart_str escaped = {0};
    zval str_zval;
    ZVAL_STR(&str_zval, str);
    php_json_encode(&escaped, &str_zval, 0);
    smart_str_0(&escaped);
    result = webview_return(w, id, status, ZSTR_VAL(escaped.s));
    smart_str_free(&escaped);
    return result;
}

WEBVIEW_API webview_error_t php_webview_return_exception(webview_t w, const char *id, int status, zend_object *exception)
{
    webview_error_t result;
    zval exception_zval;
    ZVAL_OBJ(&exception_zval, exception);
    zend_string *error_str = zval_try_get_string_func(&exception_zval);
    if (error_str->len == 0) {
        result = webview_return(w, id, 1, "\"Error occurred in PHP\"");
    } else {
        result = php_webview_return_zend_string(w, id, 1, error_str, 1);
    }
    zend_string_release(error_str);
    return result;
}

WEBVIEW_API void php_webview_binding_dtor(zval *zv)
{
    php_webview_binding_context_t *context = (php_webview_binding_context_t *)Z_PTR_P(zv);
    php_webview_callback_t *binding = context->binding;

    // Release callback references
    if (binding->fci.object) {
        GC_DELREF(binding->fci.object);
    }
    zval_ptr_dtor(&binding->fci.function_name);

    efree(binding);
    efree(context);
}

WEBVIEW_API void php_webview_binding_callback(const char *id, const char *req, void *arg)
{
    php_webview_binding_context_t *context = (php_webview_binding_context_t *)arg;
    php_webview_callback_t *binding = context->binding;
    zval args[2];
    zval retval;
    webview_error_t result = WEBVIEW_ERROR_OK;

    ZVAL_STRING(&args[0], id);
    ZVAL_STRING(&args[1], req);

    binding->fci.retval = &retval;
    binding->fci.param_count = 2;
    binding->fci.params = args;

    ZVAL_NULL(&retval);

    if (zend_call_function(&binding->fci, &binding->fcc) == SUCCESS) {
        if (EG(exception)) {
            // Handle exception thrown in PHP function
            zend_object *exception = EG(exception);
            zend_exception_error(exception, E_WARNING);
            php_webview_return_exception(context->webview, id, 1, exception);
            zend_clear_exception();
        } else {
            if (!Z_ISNULL(retval)) {
                // Handle normal return value - convert to string and return to webview
                zend_string *result_str = zval_get_string(&retval);
                result = php_webview_return_zend_string(context->webview, id, 0, result_str, 0);
                zend_string_release(result_str);
            }
        }
    } else {
        // Return error if PHP function call failed
        result = webview_return(context->webview, id, 1, "\"PHP function call failed\"");
    }

    zval_ptr_dtor(&retval);

    zval_ptr_dtor(&args[0]);
    zval_ptr_dtor(&args[1]);

    if (result != WEBVIEW_ERROR_OK) {
        php_error_docref(NULL, E_WARNING, "Failed to return result to webview: %s", webview_error_to_string(result));
    }
}

WEBVIEW_API void php_webview_dispatch_callback(webview_t w, void *arg)
{
    php_webview_dispatch_context_t *context = (php_webview_dispatch_context_t *)arg;
    zval retval;

    context->fn->fci.retval = &retval;
    context->fn->fci.param_count = 0;
    context->fn->fci.params = NULL;

    ZVAL_NULL(&retval);

    if (zend_call_function(&context->fn->fci, &context->fn->fcc) == SUCCESS) {
        if (EG(exception)) {
            // Handle exception thrown in PHP function
            zend_exception_error(EG(exception), E_WARNING);
            zend_clear_exception();
        }
    } else {
        // Handle function call failure
        php_error_docref(NULL, E_WARNING, "Dispatch callback function call failed");
    }

    zval_ptr_dtor(&retval);

    // Free the context and callback references
    if (context->fn->fci.object) {
        GC_DELREF(context->fn->fci.object);
    }
    zval_ptr_dtor(&context->fn->fci.function_name);
    efree(context->fn);
    efree(context);
}
