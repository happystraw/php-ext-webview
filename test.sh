#!/usr/bin/env bash

set -e

PHP_DEBUG_DIR=${PHP_DEBUG_DIR:-../..}

$PHP_DEBUG_DIR/bin/phpize --force
./configure --with-php-config=$PHP_DEBUG_DIR/bin/php-config
make clean
make
make install

args=("$@")
if [ ${#args[@]} -gt 0 ]; then
  $PHP_DEBUG_DIR/bin/php "${args[@]}"
else
  $PHP_DEBUG_DIR/bin/php examples/counter.php
fi
