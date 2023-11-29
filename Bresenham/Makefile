all:
	gcc BresenhamLine.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o BresenhamLine.o
	gcc BresenhamCircle.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o BresenhamCircle.o
val:
	gcc BresenhamLine.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o BresenhamLine.o
	gcc BresenhamCircle.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o BresenhamCircle.o
clean:
	rm BresenhamLine.o
	rm BresenhamCircle.o