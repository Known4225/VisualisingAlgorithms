all:
	gcc Dijkstra.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Dijkstra.o
	gcc AStar.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o AStar.o
val:
	gcc Dijkstra.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o Dijkstra.o
	gcc AStar.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -g -Wall -o AStar.o
span:
	gcc MinSpanTree.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o MinSpanTree.o
graph:
	gcc GenericGraph.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o GenericGraph.o
depth:
	gcc DepthFirstSearch.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o DepthFirstSearch.o
clean:
	rm Dijkstra.o
	rm AStar.o
	rm MinSpanTree.o
	rm GenericGraph.o