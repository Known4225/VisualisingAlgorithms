all:
	gcc Mergesort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -o Mergesort.o
	gcc Heapsort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Heapsort.o
	gcc Quicksort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Quicksort.o
val:
	gcc Mergesort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o Mergesort.o
	gcc Heapsort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o Heapsort.o
	gcc Quicksort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o Quicksort.o
clean:
	rm Heapsort.o
	rm Quicksort.o
	rm Mergesort.o