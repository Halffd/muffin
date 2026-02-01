#!/bin/bash

# Environment variables for the custom muffin installation
export PATH=$HOME/.local/cinnamon/bin:$PATH
export LD_LIBRARY_PATH=$HOME/.local/cinnamon/lib:$LD_LIBRARY_PATH
export XDG_DATA_DIRS=$HOME/.local/cinnamon/share:/usr/share:$XDG_DATA_DIRS
export GI_TYPELIB_PATH=$HOME/.local/cinnamon/lib/girepository-1.0:$GI_TYPELIB_PATH
export PKG_CONFIG_PATH=$HOME/.local/cinnamon/lib/pkgconfig:$PKG_CONFIG_PATH

# Run muffin with the custom installation
exec $HOME/.local/cinnamon/bin/muffin "$@"