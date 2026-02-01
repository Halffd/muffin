#!/bin/bash
set -e

BUILD_DIR="builddir"
PREFIX="$HOME/.local/cinnamon-fork"
JOBS=$(nproc)
BUILD_TYPE="release"

meson_setup() {
  meson setup "$BUILD_DIR" . \
    --prefix="$PREFIX" \
    --buildtype="$BUILD_TYPE"
}

build() {
  ninja -C "$BUILD_DIR" -j "$JOBS"
}

install() {
  meson install -C "$BUILD_DIR" --no-rebuild
}

clean() {
  rm -rf "$BUILD_DIR"
}

run_nested() {
  export LD_LIBRARY_PATH="$PREFIX/lib:$PREFIX/lib64"
  export GI_TYPELIB_PATH="$PREFIX/lib/girepository-1.0"
  export XDG_DATA_DIRS="$PREFIX/share:$XDG_DATA_DIRS"

  "$BUILD_DIR/src/muffin" --nested --x11
}

case "$1" in
  clean) clean ;;
  build) meson_setup && build ;;
  install) install ;;
  run) build && run_nested ;;
  *) clean && meson_setup && build && install ;;
esac
