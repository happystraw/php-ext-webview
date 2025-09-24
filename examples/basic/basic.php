<?php

/**
 * This example shows how to create a basic webview.
 */

declare(strict_types=1);

use Webview\Webview;
use Webview\WebviewHint;

$w = new Webview(true);
$w->setTitle('Base Webview');
$w->setSize(480, 320, WebviewHint::NONE);
$w->setHtml('<h1>Hello from 🐘 PHP '. PHP_VERSION . '!</h1>');
$w->run();
