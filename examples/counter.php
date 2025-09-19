#!/home/happystraw/Projects/library/php-8.3-debug/bin/php
<?php

use Webview\Webview;
use Webview\WebviewHint;

$w = new Webview(true);
$w->setTitle('Counter');
$w->setSize(480, 320, WebviewHint::MIN);
$w->setHtml(file_get_contents(__DIR__ . '/counter.html'));
$w->bind(
    'count',
    function (string $id, string $req) use ($w) {
        static $counter = 0;
        [$num] = (array)\json_decode($req, true);
        $counter += $num;
        return \json_encode(['result' => $counter]);
    }
);
$w->run();
