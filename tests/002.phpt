--TEST--
Check no memory leaks with webview extension
--EXTENSIONS--
webview
--FILE--
<?php
$w = new Webview\Webview();
$w->bind('php_version', function() {
    return PHP_VERSION;
});
$w->bind('php_sapi_name', function() {
    return PHP_SAPI;
});
echo 'OK';
?>
--EXPECT--
Overriding existing handler for signal 10. Set JSC_SIGNAL_FOR_GC if you want WebKit to use a different signal
OK
Warning: zend_signal: handler was replaced for signal (10) after startup in Unknown on line 0

