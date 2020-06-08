#!/bin/sh

find . -path ./build -prune -o \( -name '*.hpp' -or -name '*.cpp' \) \
    -exec clang-format -verbose -style=file -fallback-style=none -i {} \;
