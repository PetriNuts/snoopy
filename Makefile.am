bin_PROGRAMS = snoopy2
	
	echo "SRC = \\" > src.lst
	find . -name '*.c' | sed 's/\.\///;s/\.c/\.c \\/' >> src.lst
	find . -name '*.cc' | sed 's/\.\///;s/\.cc/\.cc \\/' >> src.lst
	find . -name '*.cpp' | sed 's/\.\///;$!s/\.cpp/\.cpp \\/' >> src.lst
include src.lst
snoopy2_SOURCES = $(SRC)	