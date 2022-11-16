file manager, like `ls` in linux

## Quick build
``
chmod +x ./build.sh
./build.sh
``

## To run
``
fm
``

# Tutorial

## Overview

Without arguments, print all files in current directory. Else, it will print files in every given directory.

By default it will print only files, where `.` is NOT at the beginning.

## Options

`-a`, `--all` - print all files
`-d`, `--dirs`, `--only-files` - print only directories
`-D`, `--not-dirs` - print only NOT directories (it will override `-d` option and vice versa)
`-r`, `--reversed` - print files in reverse order
`-R`, `--recursive` - print files recursively

You can combine options, like `ls --all -d` (it will print all directories in current directory).

You can put several one-letter options in one, like `ls -ad` (same as `ls --all -d`).
