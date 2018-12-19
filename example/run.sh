#!/bin/sh

rm -rf bin/data/*
make -j4
cp ../libs/libgit2/lib/osx/libgit2.27.dylib ./bin/example.app/Contents/MacOS/
cd bin/example.app/Contents/MacOS/
install_name_tool -change @rpath/libgit2.27.dylib @executable_path/libgit2.27.dylib ./example
./example
