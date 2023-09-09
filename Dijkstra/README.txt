Visual Demonstration of Dijkstra's algorithm and the A* algorithm

Windows:
gcc Dijkstra.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o Dijkstra.exe

Dijkstra.exe

Use the mouse to move around
Use space to step through the animation

Linux: gcc Dijkstra.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Dijkstra.o

./Dijkstra.o

Keybinds:
Click and drag: move nodes around, you cannot move nodes while Dijkstra is running
Click on a node to set it as a start/end point
Press space to advance through the animation (Dijkstra's algorithm)
Press E to toggle distance numbers
Press C to clear nodes
Press R to stop Dijkstra
Press 1 to summon a node
Hold Shift and drag from a node to another to create a connection