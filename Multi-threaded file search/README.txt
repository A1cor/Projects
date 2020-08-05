#Compile and run on Linux#
Compile with "gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 -pthread pfind.c" and run the program with the following parameters:
argv[1]: search root directory.
argv[2]: search term (file name).
argv[3]: number of searching threads to be used for the search.