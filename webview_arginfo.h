/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e2961fabde5432b002996204db43a5181280d0e7 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Webview_Webview___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, debug, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Webview_Webview___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_run, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Webview_Webview_terminate arginfo_class_Webview_Webview_run

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_setTitle, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, title, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_setSize, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO_WITH_DEFAULT_VALUE(0, hint, Webview\\WebviewHint, 0, "Webview\\WebviewHint::NONE")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_navigate, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_setHtml, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, html, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_init, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, js, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Webview_Webview_eval arginfo_class_Webview_Webview_init

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_bind, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_unbind, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_return, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, status, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, result, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_dispatch, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Webview_Webview_maximize arginfo_class_Webview_Webview_run

#define arginfo_class_Webview_Webview_minimize arginfo_class_Webview_Webview_run

#define arginfo_class_Webview_Webview_restore arginfo_class_Webview_Webview_run

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_fullscreen, 0, 0, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, enable, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_Webview_Webview_hide arginfo_class_Webview_Webview_run

#define arginfo_class_Webview_Webview_show arginfo_class_Webview_Webview_run

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_isFullscreen, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Webview_Webview_isMaximized arginfo_class_Webview_Webview_isFullscreen

#define arginfo_class_Webview_Webview_isMinimized arginfo_class_Webview_Webview_isFullscreen

#define arginfo_class_Webview_Webview_isVisible arginfo_class_Webview_Webview_isFullscreen

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Webview_Webview_version, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Webview_Webview, __construct);
ZEND_METHOD(Webview_Webview, __destruct);
ZEND_METHOD(Webview_Webview, run);
ZEND_METHOD(Webview_Webview, terminate);
ZEND_METHOD(Webview_Webview, setTitle);
ZEND_METHOD(Webview_Webview, setSize);
ZEND_METHOD(Webview_Webview, navigate);
ZEND_METHOD(Webview_Webview, setHtml);
ZEND_METHOD(Webview_Webview, init);
ZEND_METHOD(Webview_Webview, eval);
ZEND_METHOD(Webview_Webview, bind);
ZEND_METHOD(Webview_Webview, unbind);
ZEND_METHOD(Webview_Webview, return);
ZEND_METHOD(Webview_Webview, dispatch);
ZEND_METHOD(Webview_Webview, maximize);
ZEND_METHOD(Webview_Webview, minimize);
ZEND_METHOD(Webview_Webview, restore);
ZEND_METHOD(Webview_Webview, fullscreen);
ZEND_METHOD(Webview_Webview, hide);
ZEND_METHOD(Webview_Webview, show);
ZEND_METHOD(Webview_Webview, isFullscreen);
ZEND_METHOD(Webview_Webview, isMaximized);
ZEND_METHOD(Webview_Webview, isMinimized);
ZEND_METHOD(Webview_Webview, isVisible);
ZEND_METHOD(Webview_Webview, version);


static const zend_function_entry class_Webview_WebviewException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_Webview_WebviewHint_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_Webview_Webview_methods[] = {
	ZEND_ME(Webview_Webview, __construct, arginfo_class_Webview_Webview___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, __destruct, arginfo_class_Webview_Webview___destruct, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, run, arginfo_class_Webview_Webview_run, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, terminate, arginfo_class_Webview_Webview_terminate, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, setTitle, arginfo_class_Webview_Webview_setTitle, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, setSize, arginfo_class_Webview_Webview_setSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, navigate, arginfo_class_Webview_Webview_navigate, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, setHtml, arginfo_class_Webview_Webview_setHtml, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, init, arginfo_class_Webview_Webview_init, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, eval, arginfo_class_Webview_Webview_eval, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, bind, arginfo_class_Webview_Webview_bind, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, unbind, arginfo_class_Webview_Webview_unbind, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, return, arginfo_class_Webview_Webview_return, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, dispatch, arginfo_class_Webview_Webview_dispatch, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, maximize, arginfo_class_Webview_Webview_maximize, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, minimize, arginfo_class_Webview_Webview_minimize, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, restore, arginfo_class_Webview_Webview_restore, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, fullscreen, arginfo_class_Webview_Webview_fullscreen, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, hide, arginfo_class_Webview_Webview_hide, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, show, arginfo_class_Webview_Webview_show, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, isFullscreen, arginfo_class_Webview_Webview_isFullscreen, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, isMaximized, arginfo_class_Webview_Webview_isMaximized, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, isMinimized, arginfo_class_Webview_Webview_isMinimized, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, isVisible, arginfo_class_Webview_Webview_isVisible, ZEND_ACC_PUBLIC)
	ZEND_ME(Webview_Webview, version, arginfo_class_Webview_Webview_version, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Webview_WebviewException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Webview", "WebviewException", class_Webview_WebviewException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_Webview_WebviewHint(void)
{
	zend_class_entry *class_entry = zend_register_internal_enum("Webview\\WebviewHint", IS_LONG, class_Webview_WebviewHint_methods);

	zval enum_case_NONE_value;
	ZVAL_LONG(&enum_case_NONE_value, 0);
	zend_enum_add_case_cstr(class_entry, "NONE", &enum_case_NONE_value);

	zval enum_case_MIN_value;
	ZVAL_LONG(&enum_case_MIN_value, 1);
	zend_enum_add_case_cstr(class_entry, "MIN", &enum_case_MIN_value);

	zval enum_case_MAX_value;
	ZVAL_LONG(&enum_case_MAX_value, 2);
	zend_enum_add_case_cstr(class_entry, "MAX", &enum_case_MAX_value);

	zval enum_case_FIXED_value;
	ZVAL_LONG(&enum_case_FIXED_value, 3);
	zend_enum_add_case_cstr(class_entry, "FIXED", &enum_case_FIXED_value);

	return class_entry;
}

static zend_class_entry *register_class_Webview_Webview(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "Webview", "Webview", class_Webview_Webview_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
