#!/bin/sh

if [[ -n $2 ]]; then
    find $1 -path ${1}/build -prune -o \( -name '*.hpp' -or -name '*.cpp' \) \
        -exec clang-format -verbose -style=file -fallback-style=none $2 {} \;
else
    NUMBER_OF_CHANGES=$(find $1 -path ${1}/build -prune -o \( -name '*.hpp' -or -name '*.cpp' \) \
        -exec clang-format -verbose -style=file -fallback-style=none -output-replacements-xml {} \; \
      | grep "<replacement offset" \
      | wc -l)
    if [ ${NUMBER_OF_CHANGES} -ne 0 ]; then
      echo "Code format changes needed"
      exit 1
    else
      echo "No format changes needed"
      exit 0
    fi
fi
