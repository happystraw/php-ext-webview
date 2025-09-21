<?php

/**
 * This example shows how to create a basic HTTP server.
 */

if (!extension_loaded('swow')) {
    echo 'Swow extension is not loaded';
    exit(1);
}

use Swow\Buffer;
use Swow\Coroutine;
use Swow\Http\Parser;
use Swow\Http\ParserException;
use Swow\Socket;
use Swow\SocketException;

use Webview\Webview;
use Webview\WebviewHint;

function start_server() {
    $host = getenv('SERVER_HOST') ?: '127.0.0.1';
    $port = (int) (getenv('SERVER_PORT') ?: 9764);
    $backlog = (int) (getenv('SERVER_BACKLOG') ?: 8192);
    $multi = (bool) (getenv('SERVER_MULTI') ?: false);
    $bindFlag = Socket::BIND_FLAG_NONE;

    $server = new Socket(Socket::TYPE_TCP);
    if ($multi) {
        $bindFlag |= Socket::BIND_FLAG_REUSEPORT;
    }
    $server->bind($host, $port, $bindFlag)->listen($backlog);
    while (true) {
        try {
            $connection = $server->accept();
        } catch (SocketException $exception) {
            break;
        }
        Coroutine::run(static function () use ($connection): void {
            $buffer = new Buffer(Buffer::COMMON_SIZE);
            $parser = (new Parser())->setType(Parser::TYPE_REQUEST)
                ->setEvents(Parser::EVENT_URL | Parser::EVENT_BODY);
            $parsedOffset = 0;
            $body = null;
            try {
                while (true) {
                    $length = $connection->recv($buffer, $buffer->getLength());
                    if ($length === 0) {
                        break;
                    }
                    while (true) {
                        $parsedOffset += $parser->execute($buffer, $parsedOffset);
                        if ($parser->getEvent() === $parser::EVENT_NONE) {
                            $buffer->truncateFrom($parsedOffset);
                            $parsedOffset = 0;
                            break; /* goto recv more data */
                        }
                        if ($parser->getEvent() === Parser::EVENT_BODY) {
                            $body ??= new Buffer(Buffer::COMMON_SIZE);
                            $body->write(0, $buffer, $parser->getDataOffset(), $parser->getDataLength());
                        }
                        if ($parser->getEvent() === Parser::EVENT_URL) {
                            $url = new Buffer(Buffer::COMMON_SIZE);
                            $url->write(0, $buffer, $parser->getDataOffset(), $parser->getDataLength());
                            $info = parse_url($url->toString());
                        }
                        if ($parser->isCompleted()) {
                            $body ??= new Buffer(Buffer::COMMON_SIZE);
                            $str = 'Hello from PHP HTTP Server!';
                            $str .= '<br>--------------------------------';
                            $str .= '<br>URL: ' . ($url ?? $url->toString());
                            $str .= '<br>Method: ' . $parser->getMethod();
                            $str .= '<br>Path: ' . ($info['path'] ?? '');
                            $str .= '<br>Query: ' . ($info['query'] ?? '');
                            $body->append($str, $body->getLength(), strlen($str));

                            $response = sprintf(
                                "HTTP/1.1 200 OK\r\n" .
                                "Connection: %s\r\n" .
                                "Content-Type: text/html\r\n" .
                                "Content-Length: %d\r\n\r\n" .
                                '%s',
                                $parser->shouldKeepAlive() ? 'Keep-Alive' : 'Closed',
                                $body ? $body->getLength() : 0,
                                $body ?: ''
                            );
                            $connection->send($response);
                            $body?->clear();
                            break; /* goto recv more data */
                        }
                    }
                    if (!$parser->shouldKeepAlive()) {
                        break;
                    }
                }
            } catch (SocketException $exception) {
                echo "No.{$connection->getFd()} goaway! {$exception->getMessage()}" . PHP_EOL;
            } catch (ParserException $exception) {
                echo "No.{$connection->getFd()} parse error! {$exception->getMessage()}" . PHP_EOL;
            }
            $connection->close();
        });
    }
}

function start_webview() {
    $host = getenv('SERVER_HOST') ?: '127.0.0.1';
    $port = (int) (getenv('SERVER_PORT') ?: 9764);

    $w = new Webview(true);
    $w->setTitle('HTTP Server');
    $w->setSize(480, 320, WebviewHint::NONE);
    $w->navigate('http://' . $host . ':' . $port . '/?name=John&age=30');
    $w->run();
}

// cli args with server means start the server
if (isset($argv[1]) && $argv[1] === 'server') {
    echo '[server] Starting server...' . PHP_EOL;
    start_server();
    echo '[server] Server started...' . PHP_EOL;
} else {
    echo '[main] Starting server in background...' . PHP_EOL;

    $isWindows = PHP_OS_FAMILY === 'Windows';
    $php = PHP_BINARY;
    $cmd = $isWindows
        ? ($php . ' ' . __FILE__ . ' server')
        : ($php . ' ' . escapeshellarg(__FILE__) . ' server');

    // use proc_open to start the http server in background
    $descriptorSpec = [
        // 0 => ['pipe', 'r'],
        // 1 => ['pipe', 'w'],
        // 2 => ['pipe', 'w'],
    ];
    // fixme: sapi micro should customize the script to run it self in the background
    $options = $isWindows ? ['bypass_shell' => true] : [];
    $process = proc_open($cmd, $descriptorSpec, $pipes, options: $options);
    // wait for the server to start
    $pid = 0;
    while (true) {
        $status = proc_get_status($process);
        if ($status['running'] === true) {
            $pid = $status['pid'];
            break;
        }
        sleep(1);
    }
    echo '[main] Server started in background...' . PHP_EOL;

    // start the webview
    echo '[main] Running webview...' . PHP_EOL;
    start_webview();
    echo '[main] Webview quited...' . PHP_EOL;
    // kill the server process
    echo '[main] Killing server process...' . PHP_EOL;
    proc_terminate($process, 15);
    // posix_kill($pid, 15);
    echo '[main] Server process killed...' . PHP_EOL;
    proc_close($process);
}
