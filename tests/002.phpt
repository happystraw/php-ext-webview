--TEST--
Check webview version
--EXTENSIONS--
webview
--FILE--
<?php
$version = \Webview\Webview::version();

var_dump(phpversion('webview'));
var_dump($version['ext_version']);
?>
--EXPECT--
string(5) "0.3.1"
string(5) "0.3.1"
