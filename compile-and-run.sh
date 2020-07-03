if [ $# -ne 1 ]; then 
	echo "Provide task number"
else
	echo "Watching $1.c..."
	ls $1.c | entr -s "gcc -o $1.o $1.c && ./$1.o"
fi
