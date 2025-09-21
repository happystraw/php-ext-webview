# Webview Extension for PHP

A PHP native extension based on [webview/webview](https://github.com/webview/webview) that provides lightweight cross-platform embedded browser view (Webview) capabilities, with extended window control features (minimize, maximize, unmaximize, unminimize, hide/show, fullscreen, state queries, etc.).

By combining with [spc](https://github.com/crazywhalecc/static-php-cli) and [phpmicro](https://github.com/dixyes/phpmicro) to build statically compiled PHP binaries, you can easily create single-file GUI applications.

## Installation

```bash
phpize
./configure --enable-webview
make -j
make install
```

## Features

- **Cross-platform support**: Windows, macOS, and Linux
- **Lightweight**: Based on native webview components
- **Window management**:
  - `maximize()` - Maximize window
  - `minimize()` - Minimize window
  - `unmaximize()` - Restore from maximized state
  - `unminimize()` - Restore from minimized state
  - `hide()` / `show()` - Hide/show window
  - `fullscreen()` - Toggle fullscreen mode
- **State queries**: Check if window is maximized, minimized, visible, or in fullscreen

## TODO

- [ ] SPC Linux static binary build with link system gtk4 and webkitgtk
- [ ] Complete documentation and examples
