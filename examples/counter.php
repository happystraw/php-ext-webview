#!/home/happystraw/Projects/library/php-8.3-debug/bin/php
<?php

use Webview\Webview;
use Webview\WebviewHint;

$workerChan = new \parallel\Channel();
$appChan = new \parallel\Channel();

echo PHP_EOL;
$worker = static function (int $id, \parallel\Channel $workerChan, \parallel\Channel $appChan) {
    echo date('[Y-m-d H:i:s]')," - [WorkerID: $id] Worker started", PHP_EOL;
    // TODO: do long time exec
    \Swow\Coroutine::run(
        function () use ($id, $workerChan) {
            while (TRUE) {
                $msg = $workerChan->recv();
                echo date('[Y-m-d H:i:s]')," - [WorkerID: $id] Received from app: ", var_dump($msg), PHP_EOL;
                if ($msg === 'quit') {
                    break;
                }

                \Swow\Coroutine::run(
                    function () use ($id, $workerChan) {
                        // Simulate some async work
                        file_get_contents('https://httpbin.org/delay/2');
                        echo date('[Y-m-d H:i:s]')," - [WorkerID: $id] Work done, sending back to app", PHP_EOL;
                    }
                );
            }
        }
    );
    \Swow\Sync\waitAll();
    echo date('[Y-m-d H:i:s]')," - [WorkerID: $id] Worker stopped", PHP_EOL;
};
$thread1 = new \parallel\Runtime();
// $thread2 = new \parallel\Runtime();
$thread1->run($worker, [0, $workerChan, $appChan]);
// $thread2->run($worker, [1, $workerChan, $appChan]);

$w = new Webview(TRUE);
$w->setTitle('Counter');
$w->setSize(480, 320, WebviewHint::FIXED);
$w->setHtml(file_get_contents(__DIR__ . '/counter.html'));
$w->bind(
    'count',
    function (string $id, string $req) use ($w, $workerChan) {
        static $counter = 0;
        [$num] = (array)\json_decode($req, TRUE);
        $counter += $num;
        $w->dispatch(
            function () use ($workerChan) {
                // $workerChan->send("Counter updated");
                sleep(1);
                echo date('[Y-m-d H:i:s]')," - [App] Notifying worker", PHP_EOL;
            }
        );
        return \json_encode(['result' => $counter]);
    }
);
$w->run();
$workerChan->send('quit');
// $workerChan->send('quit');
$thread1->close();
// $thread2->close();
$workerChan->close();
$appChan->close();
