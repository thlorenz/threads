#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
GENERATOR=${1:-make}

git submodule init
git submodule update

./tools/gyp/gyp --depth=$DIR -Icommon.gypi \
  --generator-output=$DIR/out/ -Goutput_dir=$DIR/out -f $GENERATOR threads.gyp
