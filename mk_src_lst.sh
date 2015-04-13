#!/bin/sh

# generates the list of all sources (all found files with .cpp extension)
# script by alextov

echo "SRC = \\" > src.lst
find . -name '*.c' | sed 's/\.\///;s/\.c/\.c \\/' >> src.lst
find . -name '*.cc' | sed 's/\.\///;s/\.cc/\.cc \\/' >> src.lst
find . -name '*.cpp' | sed 's/\.\///;$!s/\.cpp/\.cpp \\/' >> src.lst
