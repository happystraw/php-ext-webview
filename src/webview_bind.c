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

static webview_error_t php_webview_return_zval(webview_t w, const char *id, zval *value)
{
    webview_error_t result;
    smart_str json = {0};
    zend_result encode_result;

    if (Z_ISNULL_P(value)) {
        return webview_return(w, id, 0, "");
    }

    encode_result = php_json_encode(&json, value, 0);
    if (encode_result == SUCCESS && JSON_G(error_code) == PHP_JSON_ERROR_NONE && json.s) {
        smart_str_0(&json);
        result = webview_return(w, id, 0, ZSTR_VAL(json.s));
    } else if (EG(exception)) {
        zend_object *exception = EG(exception);
        zend_exception_error(exception, E_WARNING);
        result = php_webview_return_exception(w, id, 1, exception);
        zend_clear_exception();
    } else {
        result = webview_return(w, id, 1, "\"PHP binding callback return value is not JSON encodable\"");
    }

    smart_str_free(&json);
    return result;
}

static zend_result php_webview_decode_binding_args(zval *args, const char *req)
{
    ZVAL_UNDEF(args);

    if (php_json_decode(args, req, strlen(req), true, PHP_JSON_PARSER_DEFAULT_DEPTH) != SUCCESS) {
        if (Z_TYPE_P(args) != IS_UNDEF) {
            zval_ptr_dtor(args);
        }
        ZVAL_UNDEF(args);
        return FAILURE;
    }

    if (Z_TYPE_P(args) != IS_ARRAY) {
        zval_ptr_dtor(args);
        ZVAL_UNDEF(args);
        return FAILURE;
    }

    return SUCCESS;
}

WEBVIEW_API void php_webview_callback_addref(php_webview_callback_t *callback)
{
    Z_TRY_ADDREF(callback->fci.function_name);
    if (callback->fcc.object) {
        GC_ADDREF(callback->fcc.object);
    }
}

WEBVIEW_API void php_webview_callback_release(php_webview_callback_t *callback)
{
    zval_ptr_dtor(&callback->fci.function_name);
    if (callback->fcc.object) {
        OBJ_RELEASE(callback->fcc.object);
    }
}

WEBVIEW_API void php_webview_binding_context_release(php_webview_binding_context_t *context)
{
    php_webview_callback_t *binding = context->binding;

    php_webview_callback_release(binding);

    efree(binding);
    efree(context);
}

WEBVIEW_API void php_webview_binding_callback(const char *id, const char *req, void *arg)
{
    php_webview_binding_context_t *context = (php_webview_binding_context_t *)arg;
    php_webview_callback_t *binding = context->binding;
    zval args[2];
    zval decoded_args;
    zval retval;
    uint32_t param_count = 0;
    webview_error_t result = WEBVIEW_ERROR_OK;

    ZVAL_UNDEF(&decoded_args);
    ZVAL_UNDEF(&retval);

    if (php_webview_decode_binding_args(&decoded_args, req) != SUCCESS) {
        result = webview_return(context->webview, id, 1, "\"PHP binding callback arguments are not a JSON array\"");
        goto cleanup;
    }

    if (context->bind_mode == PHP_WEBVIEW_BIND_MODE_AUTO) {
        ZVAL_COPY(&args[0], &decoded_args);
        param_count = 1;
    } else {
        ZVAL_STRING(&args[0], id);
        ZVAL_COPY(&args[1], &decoded_args);
        param_count = 2;
    }

    binding->fci.retval = &retval;
    binding->fci.param_count = param_count;
    binding->fci.params = args;

    ZVAL_NULL(&retval);

    if (zend_call_function(&binding->fci, &binding->fcc) == SUCCESS) {
        if (EG(exception)) {
            // Handle exception thrown in PHP function
            zend_object *exception = EG(exception);
            zend_exception_error(exception, E_WARNING);
            result = php_webview_return_exception(context->webview, id, 1, exception);
            zend_clear_exception();
        } else if (context->bind_mode == PHP_WEBVIEW_BIND_MODE_AUTO) {
            result = php_webview_return_zval(context->webview, id, &retval);
        }
    } else {
        // Return error if PHP function call failed
        result = webview_return(context->webview, id, 1, "\"PHP function call failed\"");
    }

cleanup:
    if (Z_TYPE(retval) != IS_UNDEF) {
        zval_ptr_dtor(&retval);
    }

    for (uint32_t i = 0; i < param_count; i++) {
        zval_ptr_dtor(&args[i]);
    }

    if (Z_TYPE(decoded_args) != IS_UNDEF) {
        zval_ptr_dtor(&decoded_args);
    }

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
    php_webview_callback_release(context->fn);
    efree(context->fn);
    efree(context);
}
