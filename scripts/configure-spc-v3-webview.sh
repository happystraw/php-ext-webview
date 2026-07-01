#!/usr/bin/env bash

set -euo pipefail

WEBVIEW_REF="${WEBVIEW_REF:-main}"
WEBVIEW_REPO="${WEBVIEW_REPO:-https://github.com/happystraw/php-ext-webview}"

yaml_quote() {
  local value="${1//\'/\'\'}"
  printf "'%s'" "${value}"
}

mkdir -p config/pkg/ext config/artifact

cat > config/pkg/ext/ext-webview.yml <<'YAML'
ext-webview:
  type: php-extension
  artifact: webview
  lang: cpp
  frameworks:
    - WebKit
  php-extension:
    arg-type: enable
YAML

cat > config/artifact/webview.yml <<YAML
webview:
  metadata:
    license-files:
      - LICENSE
    license: MIT
  source:
    type: git
    url: $(yaml_quote "${WEBVIEW_REPO}")
    rev: $(yaml_quote "${WEBVIEW_REF}")
    extract: php-src/ext/webview
YAML

echo "Configured StaticPHP v3 webview package"
echo "webview repo: ${WEBVIEW_REPO}"
echo "webview ref: ${WEBVIEW_REF}"
