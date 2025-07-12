#!/usr/bin/env bash

# set -e

set -ux
set -o pipefail

SEARCH_DIR="/media/totto/Totto_old/Edit-4/Conan-Sub"

readarray -d '' FILES < <(find "$SEARCH_DIR" -type f -name "*.ass" -print0)

for FILE in "${FILES[@]}"; do
    cat "$FILE" | ./build_release/ass_parser check - -n -l warn >>normal.txt 2>>error.txt
    EXIT_CODE="$?"

    if [ "$EXIT_CODE" -eq 0 ]; then
        echo "$FILE" >>normal.txt
    else
        echo "$FILE" >>error.txt
    fi

done

# iconv -f UTF-8 -t UTF-8 '/media/totto/Totto_old/Edit-4/Conan-Sub/AllSubs/Conan 997.ass' -o /dev/null

# head -c 12126784  '/media/totto/Totto_old/Edit-4/Conan-Sub/AllSubs/Conan 997.ass' | tail -c 200

## ./build_release/ass_parser check '/media/totto/Totto_old/Edit-4/Conan-Sub/AllSubs/Conan 1111.ass' -n -l warn
