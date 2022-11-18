# Description
file manager, like `ls` in linux

## Quick build
``
chmod +x ./build.sh\
./build.sh
``

## To run
``
fm
``

# Tutorial

## Overview

Without arguments, print all files in current directory.

By default it will print only files, where `.` is NOT at the beginning.

## Arguments

You can provide some directories, not starting with `-`.\
It will print all files in every given directory.

## Options

`-a`, `--all` - print all files\
`-d`, `--dirs`, `--only-files` - print only directories\
`-nd`, `--not-dirs` - print only NOT directories (it will override `-d` option and vice versa)\
`-r`, `--reversed` - print files in reverse order\
`-R`, `--recursive` - print files recursively

You can combine options, like `fm --all -d`\
(it will print all directories in current directory).

You can put several one-letter options in one,\
like `fm -ad` (same as `fm --all -d`).

## Errors

### Error types

`[!]` - Normal errors\
`[#]` - Internal errors\
`[~]` - Loadings
