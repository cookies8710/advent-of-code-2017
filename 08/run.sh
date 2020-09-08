# generates a C program from 'input', compiles it and runs
./generate.sh | tee generated.c | gcc -o generated.o  -x c - && ./generated.o
