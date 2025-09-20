<?php

use Webview\Webview;
use Webview\WebviewHint;

$w = new Webview(true);
$w->setTitle("Webview Window Control Example");
$w->setSize(800, 600, WebviewHint::MIN);
$w->setSize(800, 600, WebviewHint::NONE);
$w->bind('maximize', function() use ($w) {
    $w->maximize();
});
$w->bind('minimize', function() use ($w) {
    $w->minimize();
});
$w->bind('restore', function() use ($w) {
    $w->restore();
});
$w->bind('minimize_restore', function() use ($w) {
    $w->minimize();
    $w->eval('setTimeout(() => { window.restore(); }, 2000);');
});
$w->bind('fullscreen', function() use ($w) {
    $w->fullscreen();
});
$w->bind('unfullscreen', function() use ($w) {
    $w->unfullscreen();
});
$w->bind('hide', function() use ($w) {
    $w->hide();
    $w->eval('setTimeout(() => { window.show(); }, 2000);');
});
$w->bind('show', function() use ($w) {
    $w->show();
});
$w->setHtml(<<<'HTML'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Window Control Example</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        button {
            margin: 5px;
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h1>Window Control Example</h1>
    <button onclick="window.maximize()">Maximize</button>
    <button onclick="window.minimize()">Minimize</button>
    <button onclick="window.restore()">Restore</button>
    <button onclick="window.minimize_restore()">Minimize (restore after 2 seconds)</button>
    <button onclick="window.fullscreen()">Fullscreen</button>
    <button onclick="window.unfullscreen()">Unfullscreen</button>
    <button onclick="window.hide()">Hide (shows again after 2 seconds)</button>
    <script>
        // onready show the window
        window.addEventListener('load', () => {
            window.show();
        });
    </script>
</body>
</html>
HTML
);
$w->hide();
$w->run();
