dnl config.m4 for extension webview

PHP_ARG_ENABLE([webview],
  [whether to enable webview support],
  [AS_HELP_STRING([--enable-webview],
    [Enable webview support])],
  [no])

if test $host_os = "linux"*; then
  PHP_ARG_WITH([webview-webkitgtk],
    [for WebKitGTK version],
    [AS_HELP_STRING([--with-webview-webkitgtk=VERSION],
      [Specify WebKitGTK version: 6.0, 4.1, or 4.0 (default: auto)])],
    [auto])
fi

if test "$PHP_WEBVIEW" != "no"; then
  dnl Check platform and configure dependencies
  case "$host_os" in
    linux*)
      dnl Linux: Check for GTK and WebKitGTK using pkg-config
      AC_MSG_CHECKING([for GTK+ and WebKitGTK libraries])
      AC_MSG_RESULT([checking])

      dnl Check if specific WebKitGTK version is requested
      case "$PHP_WEBVIEW_WEBKITGTK" in
        "6.0")
          AC_MSG_CHECKING([for WebKitGTK 6.0])
          PKG_CHECK_MODULES([WEBKITGTK], [gtk4 webkitgtk-6.0],
            [
              AC_DEFINE(HAVE_WEBKITGTK_VERSION, "6.0", [WebKitGTK version])
              AC_DEFINE(HAVE_GTK_MAJOR_VERSION, "4", [GTK major version])
              AC_MSG_RESULT([found webkitgtk-6.0 with gtk4])
            ], [
              AC_MSG_ERROR([WebKitGTK 6.0 requested but not found. Please install WebKitGTK 6.0 development packages.])
            ])
          ;;
        "4.1")
          AC_MSG_CHECKING([for WebKitGTK 4.1])
          PKG_CHECK_MODULES([WEBKITGTK], [gtk+-3.0 webkit2gtk-4.1],
            [
              AC_DEFINE(HAVE_WEBKITGTK_VERSION, "4.1", [WebKitGTK version])
              AC_DEFINE(HAVE_GTK_MAJOR_VERSION, "3", [GTK major version])
              AC_MSG_RESULT([found webkit2gtk-4.1 with gtk+-3.0])
            ], [
              AC_MSG_ERROR([WebKitGTK 4.1 requested but not found. Please install WebKitGTK 4.1 development packages.])
            ])
          ;;
        "4.0")
          AC_MSG_CHECKING([for WebKitGTK 4.0])
          PKG_CHECK_MODULES([WEBKITGTK], [gtk+-3.0 webkit2gtk-4.0],
            [
              AC_DEFINE(HAVE_WEBKITGTK_VERSION, "4.0", [WebKitGTK version])
              AC_DEFINE(HAVE_GTK_MAJOR_VERSION, "3", [GTK major version])
              AC_MSG_RESULT([found webkit2gtk-4.0 with gtk+-3.0])
            ], [
              AC_MSG_ERROR([WebKitGTK 4.0 requested but not found. Please install WebKitGTK 4.0 development packages.])
            ])
          ;;
        "auto"|*)
          AC_MSG_RESULT([auto-detecting WebKitGTK version])
          dnl Try WebKitGTK 6.0 with GTK 4 first (newest)
          PKG_CHECK_MODULES([WEBKITGTK], [gtk4 webkitgtk-6.0],
            [
              AC_DEFINE(HAVE_WEBKITGTK_VERSION, "6.0", [WebKitGTK version])
              AC_DEFINE(HAVE_GTK_MAJOR_VERSION, "4", [GTK major version])
              AC_MSG_RESULT([found webkitgtk-6.0 with gtk4])
            ], [
              dnl Try WebKitGTK 4.1 with GTK 3 (recommended)
              PKG_CHECK_MODULES([WEBKITGTK], [gtk+-3.0 webkit2gtk-4.1],
                [
                  AC_DEFINE(HAVE_WEBKITGTK_VERSION, "4.1", [WebKitGTK version])
                  AC_DEFINE(HAVE_GTK_MAJOR_VERSION, "3", [GTK major version])
                  AC_MSG_RESULT([found webkit2gtk-4.1 with gtk+-3.0])
                ], [
                  dnl Try WebKitGTK 4.0 with GTK 3 (fallback)
                  PKG_CHECK_MODULES([WEBKITGTK], [gtk+-3.0 webkit2gtk-4.0],
                    [
                      AC_DEFINE(HAVE_WEBKITGTK_VERSION, "4.0", [WebKitGTK version])
                      AC_DEFINE(HAVE_GTK_MAJOR_VERSION, "3", [GTK major version])
                      AC_MSG_RESULT([found webkit2gtk-4.0 with gtk+-3.0])
                    ], [
                      AC_MSG_ERROR([WebKitGTK not found. Please install WebKitGTK development packages (4.0, 4.1, or 6.0).])
                    ])
                ])
            ])
          ;;
      esac

      dnl Add GTK/WebKit flags
      PHP_EVAL_INCLINE($WEBKITGTK_CFLAGS)
      PHP_EVAL_LIBLINE($WEBKITGTK_LIBS, WEBVIEW_SHARED_LIBADD)

      dnl Add dl library for Linux
      PHP_ADD_LIBRARY(dl, , WEBVIEW_SHARED_LIBADD)
      ;;
    darwin*)
      dnl Add WebKit framework and dl library for macOS
      PHP_ADD_LIBRARY(dl, , WEBVIEW_SHARED_LIBADD)
      PHP_ADD_FRAMEWORK(WebKit)
      if test "$ext_shared" != "no"; then
        WEBVIEW_SHARED_LIBADD="$WEBVIEW_SHARED_LIBADD -framework WebKit"
      fi
      ;;
    *)
      AC_MSG_ERROR([Unsupported platform: $host_os])
      ;;
  esac

  dnl Set dependencies header directory
  if test -z PHP_EXT_DIR(webview); then
    PHP_ADD_INCLUDE(./deps)
  else
    PHP_ADD_INCLUDE(PHP_EXT_DIR(webview)/deps)
  fi

  dnl Define extension support
  AC_DEFINE(HAVE_WEBVIEW, 1, [Have webview support])

  dnl Substitute shared library dependencies
  PHP_SUBST(WEBVIEW_SHARED_LIBADD)

  PHP_WEBVIEW_SOURCES="webview.c"
  PHP_NEW_EXTENSION(webview, $PHP_WEBVIEW_SOURCES, $ext_shared,,-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1, cxx)
  PHP_ADD_EXTENSION_DEP(webview, json)

  dnl Enable C++ compilation for webview source
  PHP_REQUIRE_CXX()

  dnl Check if C++ standard is already defined in CXXFLAGS
  AC_MSG_CHECKING([for C++ standard in CXXFLAGS])
  PHP_CXX_COMPILE_STDCXX([11], [mandatory], [PHP_WEBVIEW_STDCXX])
  PHP_WEBVIEW_CXX_FLAGS="$PHP_WEBVIEW_STDCXX"

  dnl Add C++ source files
  PHP_WEBVIEW_CXX_SOURCES="libwebview.cc"
  if test "$ext_shared" = "no"; then
    PHP_ADD_SOURCES(PHP_EXT_DIR(webview), $PHP_WEBVIEW_CXX_SOURCES, $PHP_WEBVIEW_CXX_FLAGS)
  else
    PHP_ADD_SOURCES_X(PHP_EXT_DIR(webview), $PHP_WEBVIEW_CXX_SOURCES, $PHP_WEBVIEW_CXX_FLAGS, shared_objects_webview, yes)
  fi
fi
