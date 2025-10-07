#!/bin/sh
set -e
BIN=./filter/filter

# Build (assumes file name filter.c)
cc -Wall -Wextra -Werror filter/filter.c -o $BIN

echo 'Case1 basic replace:'
printf 'abc123abc\n' | $BIN abc

echo 'Case2 no match:'
printf 'hello world\n' | $BIN xyz

echo 'Case3 overlapping pattern (aaaa, pattern aa):'
printf 'aaaa\n' | $BIN aa

echo 'Case4 single char pattern:'
printf 'banana\n' | $BIN a

echo 'Case5 long growth (> initial buf, pattern xyz):'
python3 - <<'PY' > /tmp/long_input_filter_test.txt
print('x'*3000)
PY
cat /tmp/long_input_filter_test.txt | $BIN xyz
