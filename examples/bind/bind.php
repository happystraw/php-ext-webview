<?php

/**
 * This example shows how to bind a PHP function to a JavaScript function.
 */

declare(strict_types=1);

use Webview\Webview;
use Webview\WebviewHint;

$count = 0;

$w = new Webview(true);
$w->setTitle('Bind Webview');
$w->setSize(480, 320, WebviewHint::NONE);

$w->bind('add', function (string $id, string $req) use ($w): void {
    global $count;
    $num = \json_decode($req, true)[0] ?? $num;
    $count += $num;
    // use $w->return() to return the result to the JavaScript side
    $w->return($id, 0, \json_encode(['count' => $count]));
});
$w->bind('hello', function (string $id, string $req): string {
    $name = \json_decode($req, true)[0] ?? 'World';
    // directly return the result to the JavaScript side
    return \json_encode(['message' => 'Hello ' . $name . '. This is a message from PHP.']);
});

$w->setHtml((string)file_get_contents(__FILE__, offset: __COMPILER_HALT_OFFSET__));
$w->run();

__halt_compiler();
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bind Webview</title>
</head>
<body>
    <div>
        <input type="text" id="name" placeholder="Enter your name">
        <button onclick="say_hello()">Say Hello</button>
    </div>
    <div id="result"></div>
    <br/>
    <div>
        <button onclick="add_number(-1)">-</button>
        <span id="count">0</span>
        <button onclick="add_number(1)"> + </button>
    </div>
    <script>
        function say_hello() {
            const name = document.getElementById('name').value;
            window.hello(name).then(data => {
                document.getElementById('result').innerHTML = data.message;
            });
        }
        function add_number(num) {
            window.add(num).then(data => {
                document.getElementById('count').innerHTML = data.count;
            });
        }
    </script>
</body>
</html>
