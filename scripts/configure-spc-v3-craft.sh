#!/usr/bin/env bash

set -euo pipefail

PHP_VERSION="8.5"
EXTENSIONS="webview"
PACKAGES=""
SAPI="cli,micro"
DEBUG="false"
NO_STRIP="false"
ENABLE_ZTS="false"
ENABLE_MICRO_WIN32="false"
WITH_UPX_PACK="false"
PREFER_BINARY="true"
DOWNLOAD_PARALLEL="10"

usage() {
  cat <<'EOF'
Usage: configure-spc-v3-craft.sh [options]

Options:
  --php-version VALUE
  --extensions VALUE
  --packages VALUE
  --sapi VALUE
  --debug VALUE
  --no-strip VALUE
  --enable-zts VALUE
  --enable-micro-win32 VALUE
  --with-upx-pack VALUE
  --prefer-binary VALUE
  --download-parallel VALUE
EOF
}

require_option_value() {
  local name="$1"
  local remaining="$2"
  if (( remaining < 2 )); then
    echo "Missing value for ${name}" >&2
    exit 1
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --php-version)
      require_option_value "$1" "$#"
      PHP_VERSION="$2"
      shift 2
      ;;
    --php-version=*)
      PHP_VERSION="${1#*=}"
      shift
      ;;
    --extensions)
      require_option_value "$1" "$#"
      EXTENSIONS="$2"
      shift 2
      ;;
    --extensions=*)
      EXTENSIONS="${1#*=}"
      shift
      ;;
    --packages)
      require_option_value "$1" "$#"
      PACKAGES="$2"
      shift 2
      ;;
    --packages=*)
      PACKAGES="${1#*=}"
      shift
      ;;
    --sapi)
      require_option_value "$1" "$#"
      SAPI="$2"
      shift 2
      ;;
    --sapi=*)
      SAPI="${1#*=}"
      shift
      ;;
    --debug)
      require_option_value "$1" "$#"
      DEBUG="$2"
      shift 2
      ;;
    --debug=*)
      DEBUG="${1#*=}"
      shift
      ;;
    --no-strip)
      require_option_value "$1" "$#"
      NO_STRIP="$2"
      shift 2
      ;;
    --no-strip=*)
      NO_STRIP="${1#*=}"
      shift
      ;;
    --enable-zts)
      require_option_value "$1" "$#"
      ENABLE_ZTS="$2"
      shift 2
      ;;
    --enable-zts=*)
      ENABLE_ZTS="${1#*=}"
      shift
      ;;
    --enable-micro-win32)
      require_option_value "$1" "$#"
      ENABLE_MICRO_WIN32="$2"
      shift 2
      ;;
    --enable-micro-win32=*)
      ENABLE_MICRO_WIN32="${1#*=}"
      shift
      ;;
    --with-upx-pack)
      require_option_value "$1" "$#"
      WITH_UPX_PACK="$2"
      shift 2
      ;;
    --with-upx-pack=*)
      WITH_UPX_PACK="${1#*=}"
      shift
      ;;
    --prefer-binary)
      require_option_value "$1" "$#"
      PREFER_BINARY="$2"
      shift 2
      ;;
    --prefer-binary=*)
      PREFER_BINARY="${1#*=}"
      shift
      ;;
    --download-parallel)
      require_option_value "$1" "$#"
      DOWNLOAD_PARALLEL="$2"
      shift 2
      ;;
    --download-parallel=*)
      DOWNLOAD_PARALLEL="${1#*=}"
      shift
      ;;
    -h | --help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

yaml_quote() {
  local value="${1//\'/\'\'}"
  printf "'%s'" "${value}"
}

yaml_bool() {
  case "${1,,}" in
    1 | true | yes | on) echo "true" ;;
    *) echo "false" ;;
  esac
}

yaml_comma_list() {
  local value item
  IFS=',' read -r -a value <<< "$1"
  for item in "${value[@]}"; do
    item="${item#"${item%%[![:space:]]*}"}"
    item="${item%"${item##*[![:space:]]}"}"
    if [[ -n "${item}" ]]; then
      printf "  - %s\n" "${item}"
    fi
  done
}

cat > craft.yml <<YAML
php-version: ${PHP_VERSION}
extensions: $(yaml_quote "${EXTENSIONS}")
YAML

if [[ -n "${PACKAGES}" ]]; then
  echo "packages:" >> craft.yml
  yaml_comma_list "${PACKAGES}" >> craft.yml
fi

echo "sapi:" >> craft.yml
yaml_comma_list "${SAPI}" >> craft.yml

cat >> craft.yml <<YAML
debug: $(yaml_bool "${DEBUG}")
build-options:
  no-strip: $(yaml_bool "${NO_STRIP}")
  enable-zts: $(yaml_bool "${ENABLE_ZTS}")
  with-upx-pack: $(yaml_bool "${WITH_UPX_PACK}")
  enable-micro-win32: $(yaml_bool "${ENABLE_MICRO_WIN32}")
download-options:
  parallel: ${DOWNLOAD_PARALLEL}
  prefer-binary: $(yaml_bool "${PREFER_BINARY}")
YAML

echo "Configured StaticPHP v3 craft file: craft.yml"
