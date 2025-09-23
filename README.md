# Webview Extension for PHP

**EN** | [中文](./README_CN.md)

A native PHP extension that provides lightweight, cross-platform embedded browser capabilities with window management features. Built on top of [webview/webview](https://github.com/webview/webview), this extension enables developers to create desktop applications using web technologies.

Perfect for building single-file GUI applications when combined with [static-php-cli](https://github.com/crazywhalecc/static-php-cli) and [phpmicro](https://github.com/dixyes/phpmicro).

## Features

- **Cross-platform**: Native support for Windows, macOS, and Linux
- **Lightweight**: Minimal overhead with native webview components
- **Window Management**:
  - `maximize()` / `unmaximize()` - Window maximization control
  - `minimize()` / `unminimize()` - Window minimization control
  - `hide()` / `show()` - Window visibility control
  - `fullscreen($enable = true)` - Fullscreen mode toggle
- **State Monitoring**: Real-time window state queries (maximized, minimized, visible, fullscreen)
- **PHP Integration**: Native extension with proper error handling and type safety

> **API Reference**: See [webview.stub.php](./webview.stub.php) for complete method documentation.

## Building from Source

### Prerequisites

Before building, ensure you have the required dependencies for your platform:

- **Linux**: GTK and WebKitGTK development packages
- **macOS**: Xcode command line tools
- **Windows**: Visual Studio Build Tools

> **Detailed Requirements**: See [webview/webview](https://github.com/webview/webview) for platform-specific prerequisites.

### Linux/macOS

```bash
phpize
./configure --enable-webview
make -j$(nproc)
make install
```

### Windows

Follow the [PHP Extensions](https://github.com/php/php-windows-builder?tab=readme-ov-file#php-extensions) guide for building PHP extensions on Windows.

## Single-File Applications

Create standalone executable applications by combining this extension with [static-php-cli](https://github.com/crazywhalecc/static-php-cli) and [phpmicro](https://github.com/dixyes/phpmicro). This approach bundles PHP, the webview extension, and your application code into a single executable file.

### Prerequisites

Set up your build environment by following the [static-php-cli manual build guide](https://static-php.dev/en/guide/manual-build.html#build-locally-using-source-code).

#### 1. Setup static-php-cli

```bash
git clone https://github.com/crazywhalecc/static-php-cli
cd static-php-cli
composer install
```

#### 2. Configure Extension

Add the webview extension configuration to `config/ext.json`:

```jsonc
{
    // ... other extensions
    "webview": {
        "type": "external",
        "source": "webview",
        "frameworks": ["WebKit"],
        "cpp-extension": true
    }
    // ... other extensions
}
```

Add the webview source to `config/source.json`:

```jsonc
{
    // ... other sources
    "webview": {
        "type": "git",
        "path": "php-src/ext/webview",
        "rev": "main",
        "url": "https://github.com/happystraw/php-ext-webview",
        "license": {
            "type": "file",
            "path": "LICENSE"
        }
    }
    // ... other sources
}
```

#### 3. Generate Build Commands

Use the [CLI Build Command Generator](https://static-php.dev/en/guide/cli-generator.html) to create your build commands.

> **Important**: Include `webview` in the extensions list during the `download/build` stages.

### Windows

**Example:**

```powershell
php .\bin\spc download --with-php=8.4 --for-extensions "swow,webview" --prefer-pre-built --debug
php .\bin\spc doctor --auto-fix --debug
php .\bin\spc build --build-cli --build-micro "swow,webview" --debug --with-upx-pack
```

#### Additional Options

- **Hide Console Window**: Add `--enable-micro-win32` to create windowed applications, hiding the console window.
- **Custom Icon**: Use `--with-micro-logo=/path/to/icon.ico` to set application icon.

> **More Options**: See the [static-php-cli documentation](https://static-php.dev) for additional build configurations.

### macOS

**Example:**

```bash
./bin/spc download --with-php=8.4 --for-extensions "swow,webview" --prefer-pre-built --debug
./bin/spc doctor --auto-fix --debug
./bin/spc build \
    --build-cli \
    --build-micro \
    "swow,webview" \
    --debug
```

#### Additional Options

- **Create App Bundle**: Add `.app` suffix to create a macOS application, hiding the console window:

  ```bash
  cat micro.sfx ui.php > ui.app
  chmod +x ui.app
  ```

- **Custom Icon**: Use [FileIcon](https://github.com/mklement0/fileicon) or similar tools to set application icons

### Linux

Linux requires GTK/WebKitGTK libraries for webview functionality. Additional configuration is needed for successful linking.

To successfully link to these libraries on the system, some additional operations are required.

1. Set the environment variable `PKG_CONFIG_PATH` to the directory of `.pc` files on the current system. For Ubuntu as an example:
     - /usr/lib/x86_64-linux-gnu/pkgconfig
     - /usr/share/pkgconfig
2. Set the environment variable `SPC_EXTRA_LIBS="$(pkg-config --libs gtk4 webkitgtk-6.0)"` to link `gtk4` and `webkitgtk-6.0` libraries.

#### Prerequisites

Follow the [webview/webview](https://github.com/webview/webview) documentation to install the required dependencies.

#### Build Configuration

**Example:**

Set up environment variables for library linking:

```bash
./bin/spc download --with-php=8.4 --for-extensions "swow,webview" --prefer-pre-built --debug
./bin/spc doctor --auto-fix --debug

# use zig to compile php
./bin/spc install-pkg zig

# build with GTK/WebKit linking
SPC_EXTRA_LIBS="$(pkg-config --libs gtk4 webkitgtk-6.0)" \
PKG_CONFIG_PATH="/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig" \
SPC_TARGET="native-native" \
./bin/spc build \
    --build-cli \
    --build-micro \
    "swow,webview" \
    --debug \
    --with-upx-pack
```
