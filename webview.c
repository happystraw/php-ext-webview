/* webview extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

#include "zend_API.h"
#include "zend_enum.h"
#include "zend_exceptions.h"

#include "php_webview.h"
#include "webview_arginfo.h"

#include "webview_error.h"
#include "webview_bind.h"
#include "webview_window.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

// Global class entries
zend_class_entry *webview_hint_ce;
zend_class_entry *webview_ce;
zend_class_entry *webview_exception_ce;

// Object handlers
static zend_object_handlers webview_object_handlers;

static zend_object *php_webview_create_object(zend_class_entry *ce)
{
    php_webview_obj *intern;

    intern = (php_webview_obj*)zend_object_alloc(sizeof(php_webview_obj), ce);

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->webview = NULL;
    intern->bindings = NULL;

    intern->std.handlers = &webview_object_handlers;

    return &intern->std;
}

static void php_webview_free_object(zend_object *object)
{
    php_webview_obj *intern = php_webview_from_obj(object);

    if (intern->webview) {
        webview_destroy(intern->webview);
        intern->webview = NULL;
    }

    if (intern->bindings) {
        zend_hash_destroy(intern->bindings);
        FREE_HASHTABLE(intern->bindings);
        intern->bindings = NULL;
    }

    zend_object_std_dtor(&intern->std);
}

static inline void php_webview_throw_webview_exception(webview_error_t error, const char *message)
{
    php_webview_throw_exception(webview_exception_ce, error, message);
}

/* {{{ Webview::__construct(bool $debug = false) */
PHP_METHOD(Webview_Webview, __construct)
{
    php_webview_obj *intern;
    zend_bool debug = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(debug)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    intern->webview = webview_create(debug ? 1 : 0, NULL);
    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_MISSING_DEPENDENCY, "Failed to create webview instance");
        RETURN_THROWS();
    }

    ALLOC_HASHTABLE(intern->bindings);
    zend_hash_init(intern->bindings, 0, NULL, php_webview_binding_dtor, 0);
}
/* }}} */

/* {{{ Webview::__destruct() */
PHP_METHOD(Webview_Webview, __destruct)
{
    php_webview_obj *intern;

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    // Clear all bindings to break potential circular references
    if (intern->bindings) {
        zend_hash_clean(intern->bindings);
    }
}
/* }}} */

/* {{{ Webview::run(): void */
PHP_METHOD(Webview_Webview, run)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_run(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to run webview");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::terminate(): void */
PHP_METHOD(Webview_Webview, terminate)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_terminate(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to terminate webview");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::setTitle(string $title): void */
PHP_METHOD(Webview_Webview, setTitle)
{
    php_webview_obj *intern;
    char *title;
    size_t title_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(title, title_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_set_title(intern->webview, title);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to set title");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::setSize(int $width, int $height, WebviewHint $hint = WebviewHint::NONE): void */
PHP_METHOD(Webview_Webview, setSize)
{
    php_webview_obj *intern;
    zend_long width, height;
    zval *hint = NULL;
    zend_long hint_value = WEBVIEW_HINT_NONE;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
        Z_PARAM_OPTIONAL
        Z_PARAM_OBJECT_OF_CLASS(hint, webview_hint_ce)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    // Extract enum value if provided
    if (hint != NULL) {
        zval *backing_value = zend_enum_fetch_case_value(Z_OBJ_P(hint));
        hint_value = Z_LVAL_P(backing_value);
    }

    webview_error_t result = webview_set_size(intern->webview, (int)width, (int)height, (webview_hint_t)hint_value);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to set size");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::navigate(string $url): void */
PHP_METHOD(Webview_Webview, navigate)
{
    php_webview_obj *intern;
    char *url;
    size_t url_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(url, url_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_navigate(intern->webview, url);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to navigate");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::setHtml(string $html): void */
PHP_METHOD(Webview_Webview, setHtml)
{
    php_webview_obj *intern;
    char *html;
    size_t html_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(html, html_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_set_html(intern->webview, html);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to set HTML");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::init(string $js): void */
PHP_METHOD(Webview_Webview, init)
{
    php_webview_obj *intern;
    char *js;
    size_t js_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(js, js_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_init(intern->webview, js);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to initialize script");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::eval(string $js): void */
PHP_METHOD(Webview_Webview, eval)
{
    php_webview_obj *intern;
    char *js;
    size_t js_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(js, js_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_eval(intern->webview, js);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to evaluate script");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::bind(string $name, callable $callback): void */
PHP_METHOD(Webview_Webview, bind)
{
    php_webview_obj *intern;
    char *name;
    size_t name_len;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    php_webview_callback_t *binding;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(name, name_len)
        Z_PARAM_FUNC(fci, fcc)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    // Check if binding already exists
    if (zend_hash_str_exists(intern->bindings, name, name_len)) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Binding already exists: %s", name);
        php_webview_throw_webview_exception(WEBVIEW_ERROR_DUPLICATE, error_msg);
        RETURN_THROWS();
    }

    // Create binding structure
    binding = (php_webview_callback_t*)emalloc(sizeof(php_webview_callback_t));
    binding->fci = fci;
    binding->fcc = fcc;

    // Create callback context
    php_webview_binding_context_t *context = (php_webview_binding_context_t*)emalloc(sizeof(php_webview_binding_context_t));
    context->binding = binding;
    context->webview = intern->webview;

    // Add reference to callback
    Z_TRY_ADDREF(fci.function_name);
    if (fci.object) {
        GC_ADDREF(fci.object);
    }

    // Store context in hashtable (not just binding)
    zend_hash_str_add_ptr(intern->bindings, name, name_len, context);

    // Bind to webview with context
    webview_error_t result = webview_bind(intern->webview, name, php_webview_binding_callback, context);

    if (result != WEBVIEW_ERROR_OK) {
        // Clean up on failure
        zend_hash_str_del(intern->bindings, name, name_len);
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to bind function: %s", name);
        php_webview_throw_webview_exception(result, error_msg);
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::unbind(string $name): void */
PHP_METHOD(Webview_Webview, unbind)
{
    php_webview_obj *intern;
    char *name;
    size_t name_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(name, name_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    // Remove from webview
    webview_error_t result = webview_unbind(intern->webview, name);

    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to unbind function");
        RETURN_THROWS();
    }

    // Remove from bindings hashtable
    zend_hash_str_del(intern->bindings, name, name_len);
}
/* }}} */

/* {{{ Webview::return(string $id, int $status, string $result): void */
PHP_METHOD(Webview_Webview, return)
{
    php_webview_obj *intern;
    char *id;
    size_t id_len;
    zend_long status;
    char *result;
    size_t result_len;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_STRING(id, id_len)
        Z_PARAM_LONG(status)
        Z_PARAM_STRING(result, result_len)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t error = webview_return(intern->webview, id, (int)status, result);

    if (error != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(error, "Failed to return result to webview");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::dispatch(callable $callback): void */
PHP_METHOD(Webview_Webview, dispatch)
{
    php_webview_obj *intern;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_FUNC(fci, fcc)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    // Create callback structure
    php_webview_callback_t *fn = (php_webview_callback_t*)emalloc(sizeof(php_webview_callback_t));
    fn->fci = fci;
    fn->fcc = fcc;

    // Create dispatch context
    php_webview_dispatch_context_t *context = (php_webview_dispatch_context_t*)emalloc(sizeof(php_webview_dispatch_context_t));
    context->fn = fn;
    context->webview = intern->webview;

    // Add reference to callback
    Z_TRY_ADDREF(fci.function_name);
    if (fci.object) {
        GC_ADDREF(fci.object);
    }

    // Dispatch to webview
    webview_error_t result = webview_dispatch(intern->webview, php_webview_dispatch_callback, context);

    if (result != WEBVIEW_ERROR_OK) {
        // Clean up on failure
        if (fn->fci.object) {
            GC_DELREF(fn->fci.object);
        }
        zval_ptr_dtor(&fn->fci.function_name);
        efree(fn);
        efree(context);
        php_webview_throw_webview_exception(result, "Failed to dispatch callback");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::maximize(): void */
PHP_METHOD(Webview_Webview, maximize)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_window_maximize(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to maximize window");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::minimize(): void */
PHP_METHOD(Webview_Webview, minimize)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_window_minimize(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to minimize window");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::unmaximize(): void */
PHP_METHOD(Webview_Webview, unmaximize)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_window_unmaximize(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to unmaximize window");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::unminimize(): void */
PHP_METHOD(Webview_Webview, unminimize)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_window_unminimize(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to unminimize window");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::fullscreen(bool $enable = true): void */
PHP_METHOD(Webview_Webview, fullscreen)
{
    php_webview_obj *intern;
    zend_bool enable = 1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(enable)
    ZEND_PARSE_PARAMETERS_END();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result;
    if (enable) {
        result = webview_window_fullscreen(intern->webview);
        if (result != WEBVIEW_ERROR_OK) {
            php_webview_throw_webview_exception(result, "Failed to enter fullscreen mode");
            RETURN_THROWS();
        }
    } else {
        result = webview_window_unfullscreen(intern->webview);
        if (result != WEBVIEW_ERROR_OK) {
            php_webview_throw_webview_exception(result, "Failed to exit fullscreen mode");
            RETURN_THROWS();
        }
    }
}
/* }}} */

/* {{{ Webview::hide(): void */
PHP_METHOD(Webview_Webview, hide)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_window_hide(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to hide window");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::show(): void */
PHP_METHOD(Webview_Webview, show)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    webview_error_t result = webview_window_show(intern->webview);
    if (result != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(result, "Failed to show window");
        RETURN_THROWS();
    }
}
/* }}} */

/* {{{ Webview::isFullscreen(): bool */
PHP_METHOD(Webview_Webview, isFullscreen)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    int result = 0;
    webview_error_t error = webview_window_is_fullscreen(intern->webview, &result);
    if (error != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(error, "Failed to check fullscreen status");
        RETURN_THROWS();
    }

    RETURN_BOOL(result);
}
/* }}} */

/* {{{ Webview::isMaximized(): bool */
PHP_METHOD(Webview_Webview, isMaximized)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    int result = 0;
    webview_error_t error = webview_window_is_maximized(intern->webview, &result);
    if (error != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(error, "Failed to check maximized status");
        RETURN_THROWS();
    }

    RETURN_BOOL(result);
}
/* }}} */

/* {{{ Webview::isMinimized(): bool */
PHP_METHOD(Webview_Webview, isMinimized)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    int result = 0;
    webview_error_t error = webview_window_is_minimized(intern->webview, &result);
    if (error != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(error, "Failed to check minimized status");
        RETURN_THROWS();
    }

    RETURN_BOOL(result);
}
/* }}} */

/* {{{ Webview::isVisible(): bool */
PHP_METHOD(Webview_Webview, isVisible)
{
    php_webview_obj *intern;

    ZEND_PARSE_PARAMETERS_NONE();

    intern = Z_WEBVIEW_OBJ_P(ZEND_THIS);

    if (!intern->webview) {
        php_webview_throw_webview_exception(WEBVIEW_ERROR_INVALID_STATE, "Webview instance is not initialized");
        RETURN_THROWS();
    }

    int result = 0;
    webview_error_t error = webview_window_is_visible(intern->webview, &result);
    if (error != WEBVIEW_ERROR_OK) {
        php_webview_throw_webview_exception(error, "Failed to check visibility status");
        RETURN_THROWS();
    }

    RETURN_BOOL(result);
}
/* }}} */

/* {{{ Webview::version(): array */
PHP_METHOD(Webview_Webview, version)
{
    const webview_version_info_t *version_info;
    zval lib_version;

    ZEND_PARSE_PARAMETERS_NONE();

    version_info = webview_version();

    array_init(&lib_version);
    add_assoc_long(&lib_version, "major", version_info->version.major);
    add_assoc_long(&lib_version, "minor", version_info->version.minor);
    add_assoc_long(&lib_version, "patch", version_info->version.patch);
    add_assoc_string(&lib_version, "version_number", version_info->version_number);
    add_assoc_string(&lib_version, "pre_release", version_info->pre_release);
    add_assoc_string(&lib_version, "build_metadata", version_info->build_metadata);

    array_init(return_value);
    add_assoc_string(return_value, "ext_version", PHP_WEBVIEW_VERSION);
    add_assoc_zval(return_value, "lib_version", &lib_version);
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(webview)
{
#if defined(ZTS) && defined(COMPILE_DL_WEBVIEW)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(webview)
{
    webview_exception_ce = register_class_Webview_WebviewException(zend_ce_exception);

    webview_hint_ce = register_class_Webview_WebviewHint();

    webview_ce = register_class_Webview_Webview();
    webview_ce->create_object = php_webview_create_object;

    memcpy(&webview_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    webview_object_handlers.free_obj = php_webview_free_object;
    webview_object_handlers.offset = XtOffsetOf(php_webview_obj, std);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(webview)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(webview)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Webview support", "enabled");
    php_info_print_table_row(2, "Webview version", PHP_WEBVIEW_VERSION);
    php_info_print_table_row(2, "LibWebview version", WEBVIEW_VERSION_NUMBER);
#ifdef HAVE_WEBKITGTK_VERSION
    php_info_print_table_row(2, "WebKitGTK version", HAVE_WEBKITGTK_VERSION);
#endif
#ifdef HAVE_GTK_MAJOR_VERSION
	php_info_print_table_row(2, "GTK major version", HAVE_GTK_MAJOR_VERSION);
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ webview_module_entry */
zend_module_entry webview_module_entry = {
	STANDARD_MODULE_HEADER,
	"webview",					/* Extension name */
	NULL,							/* zend_function_entry */
	PHP_MINIT(webview),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(webview),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(webview),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(webview),			/* PHP_MINFO - Module info */
	PHP_WEBVIEW_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WEBVIEW
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(webview)
#endif
