# PHP Webview 扩展

[EN](./README.md) | **中文**

一个原生 PHP 扩展，提供轻量级、跨平台的嵌入式浏览器功能，具有窗口管理特性。基于 [webview/webview](https://github.com/webview/webview) 构建，该扩展使开发者能够使用 Web 技术创建桌面应用程序。

与 [static-php-cli](https://github.com/crazywhalecc/static-php-cli) 和 [phpmicro](https://github.com/dixyes/phpmicro) 结合使用时，非常适合构建单文件 GUI 应用程序。

## 功能特性

- **跨平台**：原生支持 Windows、macOS 和 Linux
- **轻量级**：使用原生 webview 组件，开销最小
- **窗口管理**：
  - `maximize()` / `unmaximize()` - 窗口最大化控制
  - `minimize()` / `unminimize()` - 窗口最小化控制
  - `hide()` / `show()` - 窗口可见性控制
  - `fullscreen($enable = true)` - 全屏模式切换
- **状态监控**：实时窗口状态查询（最大化、最小化、可见、全屏）
- **PHP 集成**：原生扩展，具有适当的错误处理和类型安全

> **API 参考**：查看 [webview.stub.php](./webview.stub.php) 获取完整的方法文档。

## 从源码构建

### 前置要求

在构建之前，请确保您的平台已安装所需的依赖项：

- **Linux**：GTK 和 WebKitGTK 开发包
- **macOS**：Xcode 命令行工具
- **Windows**：Visual Studio 构建工具

> **详细要求**：查看 [webview/webview](https://github.com/webview/webview) 获取平台特定的前置要求。

### Linux/macOS

```bash
phpize
./configure --enable-webview
make -j$(nproc)
make install
```

### Windows

按照 [PHP Extensions](https://github.com/php/php-windows-builder?tab=readme-ov-file#php-extensions) 指南在 Windows 上构建 PHP 扩展。

## 单文件应用程序

通过将此扩展与 [static-php-cli](https://github.com/crazywhalecc/static-php-cli) 和 [phpmicro](https://github.com/dixyes/phpmicro) 结合使用，创建独立的可执行应用程序。这种方法将 PHP、webview 扩展和您的应用程序代码打包到一个可执行文件中。

### 前置要求

按照 [static-php-cli 手动构建指南](https://static-php.dev/zh/guide/manual-build.html#%E6%89%8B%E5%8A%A8%E6%9E%84%E5%BB%BA-%E4%BD%BF%E7%94%A8%E6%BA%90%E7%A0%81) 设置您的构建环境。

#### 1. 设置 static-php-cli

```bash
git clone https://github.com/crazywhalecc/static-php-cli
cd static-php-cli
composer install
```

#### 2. 配置扩展

将 webview 扩展配置添加到 `config/ext.json`：

```jsonc
{
    // ... 其他扩展
    "webview": {
        "type": "external",
        "source": "webview",
        "frameworks": ["WebKit"],
        "cpp-extension": true
    }
    // ... 其他扩展
}
```

将 webview 源添加到 `config/source.json`：

```jsonc
{
    // ... 其他源
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
    // ... 其他源
}
```

#### 3. 生成构建命令

使用 [CLI 构建命令生成器](https://static-php.dev/zh/guide/cli-generator.html) 创建您的构建命令。

> **重要**：在 `download/build` 阶段，确保在扩展列表中包含 `webview`。

### Windows

**示例：**

```powershell
php .\bin\spc download --with-php=8.4 --for-extensions "swow,webview" --prefer-pre-built --debug
php .\bin\spc doctor --auto-fix --debug
php .\bin\spc build --build-cli --build-micro "swow,webview" --debug --with-upx-pack
```

#### 附加选项

- **隐藏控制台窗口**：添加 `--enable-micro-win32` 创建窗口化应用程序，隐藏控制台窗口。
- **自定义图标**：使用 `--with-micro-logo=/path/to/icon.ico` 设置应用程序图标。

> **更多选项**：查看 [static-php-cli 文档](https://static-php.dev) 获取其他构建配置。

### macOS

**示例：**

```bash
./bin/spc download --with-php=8.4 --for-extensions "swow,webview" --prefer-pre-built --debug
./bin/spc doctor --auto-fix --debug
./bin/spc build \
    --build-cli \
    --build-micro \
    "swow,webview" \
    --debug
```

#### 附加选项

- **创建 App Bundle**：添加 `.app` 后缀创建 macOS 应用程序，隐藏控制台窗口：

  ```bash
  cat micro.sfx webview.php > webview.app
  chmod +x webview.app
  ```

- **自定义图标**：使用 [FileIcon](https://github.com/mklement0/fileicon) 或类似工具设置应用程序图标

### Linux

Linux 需要 GTK/WebKitGTK 库来实现 webview 功能。需要额外配置才能成功链接。

要在系统上成功链接这些库，需要执行一些额外操作。

1. 将环境变量 `PKG_CONFIG_PATH` 设置为当前系统上 `.pc` 文件的目录。以 Ubuntu 为例：
     - /usr/lib/x86_64-linux-gnu/pkgconfig
     - /usr/share/pkgconfig
2. 设置环境变量 `SPC_EXTRA_LIBS="$(pkg-config --libs gtk4 webkitgtk-6.0)"` 以链接 `gtk4` 和 `webkitgtk-6.0` 库。

#### 前置要求

按照 [webview/webview](https://github.com/webview/webview) 文档安装所需的依赖项。

#### 构建配置

**示例：**

设置库链接的环境变量：

```bash
./bin/spc download --with-php=8.4 --for-extensions "swow,webview" --prefer-pre-built --debug
./bin/spc doctor --auto-fix --debug

# 使用 zig 编译 php
./bin/spc install-pkg zig

# 使用 GTK/WebKit 链接构建
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
