# Webview Extension for PHP

**EN** | [中文](./README_CN.md)

A native PHP extension that provides lightweight, cross-platform embedded browser capabilities with window management features. Built on top of [webview/webview](https://github.com/webview/webview), this extension enables developers to create desktop applications using web technologies.

Perfect for building single-file GUI applications when combined with [static-php-cli](https://github.com/crazywhalecc/static-php-cli) and [phpmicro](https://github.com/dixyes/phpmicro).

## Features

- **Support for PHP**: 8.3, 8.4, 8.5
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

## Quick Start

To quickly experience the webview extension, you can download pre-built PHP executables from the [Releases](https://github.com/happystraw/php-ext-webview/releases) page.

> This build only includes a few extensions. For more functionality, please refer to the source build and GitHub Actions build below.

### Linux/macOS

**CLI:**

```bash
# 1. Download the corresponding architecture php-cli-8.4-*.zip
# 2. Extract and set execution permissions
# 3. Run examples
chmod +x ./php
./php examples/basic.php
```

**Micro:**

```bash
# 1. Download the corresponding architecture php-micro-8.4-*.zip
# 2. Extract and set execution permissions
# 3. Merge into single file program and run
cat micro.sfx examples/basic.php > basic
chmod +x basic
./basic
```

### Windows

> **Note**: Windows version uses UPX compression and may trigger antivirus alerts. Please add to trust list or build from source.

**CLI:**

```cmd
REM 1. Download the corresponding architecture php-cli-8.4-windows-*.zip
REM 2. Extract to any directory
REM 3. Run examples
.\php.exe examples\basic.php
```

**Micro:**

```cmd
REM 1. Download the corresponding architecture php-micro-8.4-windows-*.zip
REM 2. Extract to any directory
REM 3. Merge into single file program and run
COPY /b micro.sfx examples\basic.php basic.exe
.\basic.exe
```

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

## Building with GitHub Actions

For easier building and distribution, you can fork this repository and use the pre-configured GitHub Actions workflows in this repository to automatically build PHP CLI/Micro executables with the webview extension.

### Available Workflows

- **Build for Unix**: Builds static PHP CLI/Micro executables for Linux (x86_64, aarch64) and macOS (x86_64, aarch64)
- **Build for Windows**: Builds static PHP CLI/Micro executables for Windows (x86_64)

## Static PHP CLI/Micro Build

You can use [static-php-cli](https://github.com/crazywhalecc/static-php-cli) and [phpmicro](https://github.com/dixyes/phpmicro) together to build static PHP CLI/Micro executables.

For specific operations, you can refer to the [build-unix.yml](.github/workflows/build-unix.yml) and [build-windows.yml](.github/workflows/build-windows.yml) files in the GitHub Actions workflows.
