Visual Demonstration of Dijkstra's algorithm and the A* algorithm

Windows:
gcc Dijkstra.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o Dijkstra.exe

Dijkstra.exe

Linux: gcc Dijkstra.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Dijkstra.o

./Dijkstra.o

Keybinds:
Click and drag: move nodes around, you cannot move nodes while Dijkstra is running
Click on a node to set it as a start/end point
Press space to advance through the animation (Dijkstra's algorithm)
Press C to clear nodes
Press R to stop Dijkstra
Press Q to regenerate new graph
Press 1 to summon a node
Press X to delete a node
Press D to toggle distance changing
Hold Shift and drag from a node to another to create a connection