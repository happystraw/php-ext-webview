<?php

/**
 * @generate-class-entries
 * @undocumentable
 */

namespace Webview;

/**
 * Webview exception class
 *
 * Exception codes correspond to enum values:
 *
 * -5: WEBVIEW_ERROR_MISSING_DEPENDENCY
 * -4: WEBVIEW_ERROR_CANCELED
 * -3: WEBVIEW_ERROR_INVALID_STATE
 * -2: WEBVIEW_ERROR_INVALID_ARGUMENT
 * -1: WEBVIEW_ERROR_UNSPECIFIED
 *  1: WEBVIEW_ERROR_DUPLICATE
 *  2: WEBVIEW_ERROR_NOT_FOUND
 */
final class WebviewException extends \Exception
{
}

enum WebviewHint: int
{
    case NONE = 0;
    case MIN = 1;
    case MAX = 2;
    case FIXED = 3;
}

/**
 * PHP Webview class - A tiny cross-platform webview library
 *
 * @package Webview
 */
final class Webview
{
    /**
     * Create a new webview instance
     *
     * @param bool $debug Enable developer tools if supported by the backend
     *
     * @throws WebviewException on failure
     */
    public function __construct(bool $debug = false) {}

    /**
     * Destroy the webview instance
     */
    public function __destruct() {}

    /**
     * Run the main loop until it's terminated
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function run(): void {}

    /**
     * Stop the main loop
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function terminate(): void {}

    /**
     * Set the window title
     *
     * @param string $title The new window title
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function setTitle(string $title): void {}

    /**
     * Set the window size
     *
     * @param int         $width  Window width
     * @param int         $height Window height
     * @param WebviewHint $hint   Size hint
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function setSize(int $width, int $height, WebviewHint $hint = WebviewHint::NONE): void {}

    /**
     * Navigate to the given URL
     *
     * @param string $url URL to navigate to
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function navigate(string $url): void {}

    /**
     * Load HTML content into the webview
     *
     * @param string $html HTML content
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function setHtml(string $html): void {}

    /**
     * Inject JavaScript code to be executed immediately upon loading a page
     *
     * @param string $js JavaScript code
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function init(string $js): void {}

    /**
     * Evaluate arbitrary JavaScript code
     *
     * @param string $js JavaScript code
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function eval(string $js): void {}

    /**
     * Bind a PHP function to a JavaScript function
     *
     * callback must be a callable with the following signature:
     * params:
     * - id:  a string identifier for the binding call.
     * - req: a JSON-encoded string containing the arguments passed from JavaScript.
     * returns: optional, either a JSON-encoded string to be returned to JavaScript.
     *          see return() for details.
     *
     * This method is blocked until the binding is complete,
     * next lib webview version this maybe changed.
     *
     * @param string                                                                        $name     Name of the JavaScript function
     * @param callable(string $id,string $req):string|callable(string $id,string $req):void $callback PHP callback function
     *
     * @return void
     *
     * @throws WebviewException on failure
     *
     */
    public function bind(string $name, callable $callback): void {}

    /**
     * Remove a binding created with bind()
     *
     * @param string $name Name of the binding
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function unbind(string $name): void {}

    /**
     * Responds to a binding call from the JS side.
     *
     * @param string $id     The identifier of the binding call. Pass along the value received
     *                       in the binding handler (see bind()).
     * @param int    $status A status of zero tells the JS side that the binding call was
     *                       successful; any other value indicates an error.
     * @param string $result The result of the binding call to be returned to the JS side.
     *                       This must either be a valid JSON value or an empty string for
     *                       the primitive JS value.
     *
     * @throws WebviewException on failure
     */
    public function return(string $id, int $status, string $result): void {}

    /**
     * Schedules a function to be invoked on the thread with the run/event loop.
     *
     * Notice: Use this method in running webview only.
     * If the webview is not running, a memory leak may occur
     * when the webview instance is held by a circular reference.
     *
     * Next lib webview version this maybe removed.
     *
     * @param callable():void $callback The function to invoke
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function dispatch(callable $callback): void {}

    /**
     * Maximize the webview window
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function maximize(): void {}

    /**
     * Minimize the webview window
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function minimize(): void {}

    /**
     * Unmaximize the webview window
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function unmaximize(): void {}

    /**
     * Unminimize the webview window
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function unminimize(): void {}

    /**
     * Set fullscreen mode
     *
     * @param bool $enable True to enter fullscreen mode, false to exit fullscreen mode
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function fullscreen(bool $enable = true): void {}

    /**
     * Hide the webview window
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function hide(): void {}

    /**
     * Show the webview window
     *
     * @return void
     *
     * @throws WebviewException on failure
     */
    public function show(): void {}

    /**
     * Check if the webview window is in fullscreen mode
     *
     * @return bool Returns true if window is in fullscreen mode, false otherwise
     *
     * @throws WebviewException on failure
     */
    public function isFullscreen(): bool {}

    /**
     * Check if the webview window is maximized
     *
     * @return bool Returns true if window is maximized, false otherwise
     *
     * @throws WebviewException on failure
     */
    public function isMaximized(): bool {}

    /**
     * Check if the webview window is minimized
     *
     * @return bool Returns true if window is minimized, false otherwise
     *
     * @throws WebviewException on failure
     */
    public function isMinimized(): bool {}

    /**
     * Check if the webview window is visible
     *
     * @return bool Returns true if window is visible, false otherwise
     *
     * @throws WebviewException on failure
     */
    public function isVisible(): bool {}

    /**
     * Get the library version information
     *
     * @return array{
     *      ext_version: string,
     *      lib_webview: array{
     *          major: int,
     *          minor: int,
     *          patch: int,
     *          version_number: string,
     *          pre_release: string,
     *          build_metadata: string,
     *      }
     * } Returns version information
     */
    public static function version(): array {}
}
