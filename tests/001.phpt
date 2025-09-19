--TEST--
Check if webview is loaded
--EXTENSIONS--
webview
--FILE--
<?php
if (class_exists(\Webview\Webview::class)) {
    echo 'The extension "webview" is available';
} else {
    echo 'The extension "webview" is not available';
}
?>
--EXPECT--
The extension "webview" is available
