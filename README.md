# ofxGit2

## description
openFrameworks addon for working with git repositories.

This is a work in progress and only features needed for ofPackageManager are added so far.

## Build status
[![Build Status](https://travis-ci.org/thomasgeissl/ofxGit2.svg?branch=master)](https://travis-ci.org/thomasgeissl/ofxGit2)

## Compiling libgit2
* brew install libssh2
* clone repo: git clone git@github.com:libgit2/libgit2.git
* mkdir build && cmake .. -DBUILD_SHARED_LIBS=OFF -DTHREADSAFE=ON && cmake --build .


## TODOs
* include libs for linux and win, update addon_config.mk

## license
This project is released under MIT license, please note that dependencies might be released differently.

Copyright (c) 2018 Thomas Geissl

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
