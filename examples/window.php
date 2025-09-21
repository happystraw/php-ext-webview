<?php

/**
 * This example shows how to control the window of the webview.
 */

declare(strict_types=1);

use Webview\Webview;
use Webview\WebviewHint;

$w = new Webview(true);
$w->setTitle("Webview Window Control Example");
$w->setSize(800, 600, WebviewHint::MIN);
$w->setSize(800, 600, WebviewHint::NONE);
$w->bind('maximize', fn() => $w->maximize());
$w->bind('minimize', fn() => $w->minimize());
$w->bind('restore', fn() => $w->restore());
$w->bind('minimize_restore', function() use ($w) {
    $w->minimize();
    $w->eval('setTimeout(() => { window.restore(); }, 2000);');
});
$w->bind('fullscreen', fn() => $w->fullscreen(true));
$w->bind('unfullscreen', fn() => $w->fullscreen(false));
$w->bind('hide_show', function() use ($w) {
    $w->hide();
    $w->eval('setTimeout(() => { window.show(); }, 2000);');
});
$w->bind('show', fn() => $w->show());
$w->bind('checkStatus', fn() => json_encode([
    'isFullscreen' => $w->isFullscreen(),
    'isMaximized' => $w->isMaximized(),
    'isMinimized' => $w->isMinimized(),
    'isVisible' => $w->isVisible()
]));
$w->bind('logStatus', function() use ($w) {
    echo "Current Window Status:\n";
    echo "- Fullscreen: " . ($w->isFullscreen() ? 'Yes' : 'No') . "\n";
    echo "- Maximized: " . ($w->isMaximized() ? 'Yes' : 'No') . "\n";
    echo "- Minimized: " . ($w->isMinimized() ? 'Yes' : 'No') . "\n";
    echo "- Visible: " . ($w->isVisible() ? 'Yes' : 'No') . "\n";
    echo "------------------------\n";
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
            margin: 4px;
            padding: 4px 8px;
            font-size: 16px;
            cursor: pointer;
        }
        .status-section {
            margin-top: 30px;
            padding: 20px;
            background-color: white;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
            width: 300px;
        }
        .status-display {
            font-family: monospace;
            background-color: #f8f8f8;
            padding: 10px;
            border-radius: 3px;
            margin-top: 10px;
            white-space: pre-line;
        }
        .control-section {
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="control-section">
        <h3>Window Controls</h3>
        <button onclick="window.maximize()">Maximize</button>
        <button onclick="window.minimize()">Minimize</button>
        <button onclick="window.restore()">Restore</button>
        <button onclick="window.minimize_restore()">Minimize (restore after 2 seconds)</button>
        <br>
        <button onclick="window.fullscreen()">Fullscreen</button>
        <button onclick="window.unfullscreen()">Unfullscreen</button>
        <button onclick="window.hide_show()">Hide (shows again after 2 seconds)</button>
    </div>

    <div class="status-section">
        <h3>Window Status</h3>
        <button onclick="updateStatus()">Check Status</button>
        <button onclick="logToConsole()">Log Status to Console</button>
        <div id="status" class="status-display">Click "Check Status" to see current window state</div>
    </div>

    <script>
        async function updateStatus() {
            try {
                const status = await window.checkStatus();

                let statusText = 'Current Window Status:\n';
                statusText += `Fullscreen: ${status.isFullscreen ? 'Yes' : 'No'}\n`;
                statusText += `Maximized: ${status.isMaximized ? 'Yes' : 'No'}\n`;
                statusText += `Minimized: ${status.isMinimized ? 'Yes' : 'No'}\n`;
                statusText += `Visible: ${status.isVisible ? 'Yes' : 'No'}`;

                document.getElementById('status').textContent = statusText;
            } catch (error) {
                document.getElementById('status').textContent = 'Error checking status: ' + error.message;
            }
        }

        // Log status to PHP console
        function logToConsole() {
            window.logStatus();
        }

        // Monitor window changes, auto update status (optional)
        setInterval(function () {
            updateStatus();
            logToConsole();
        }, 2000);
    </script>
</body>
</html>
HTML
);
$w->run();
