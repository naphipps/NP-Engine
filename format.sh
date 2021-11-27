#! /bin/sh

find . -type f -name "*.[ch]pp" \( -path "./test/*" -o -path "./include/*" -o -path "./src/*" \) -prune | xargs clang-format -i
