#!/bin/bash

if [[ -n $2 ]]; then
     find ${1} -type d \( -path ${1}/build -o -path ${1}/cmake-modules \) -prune -false -o \( -name '*.hpp' -or -name '*.cpp' \) \
        -exec clang-format -verbose -style=file -fallback-style=none $2 {} \;
else
    #find . -type d \( -path ${1}/build -o -path ${1}/cmake-modules \) -prune -false -o \( -name '*.hpp' -or -name '*.cpp' \)

    NUMBER_OF_CHANGES=$(find ${1} -type d \( -path ${1}/build -o -path ${1}/cmake-modules \) -prune -false -o \( -name '*.hpp' -or -name '*.cpp' \) \
        -exec clang-format -verbose -style=file -fallback-style=none -output-replacements-xml {} \; \
      | grep "<replacement offset" \
      | wc -l)
    if [ ${NUMBER_OF_CHANGES} -ne 0 ]; then
      echo "Code format changes needed. Run `make format`."
      exit 1
    else
      echo "No format changes needed"
      exit 0
    fi
fi
