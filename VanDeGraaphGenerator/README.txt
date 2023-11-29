This project generates planar graphs, I plan to add more graph-related math things. For now it can be used to make custom graphs and to visually solve graph isomorphisms

Windows:
gcc VanDeGraaphGenerator.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o VanDeGraaphGenerator.exe

VanDeGraaphGenerator.exe

Linux:
gcc VanDeGraaphGenerator.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o VanDeGraaphGenerator.o

./VanDeGraaphGenerator.o

Keybinds:
Click and drag: move nodes around
Hold space and click and drag nodes to create a connection
Press C to clear nodes
Press X to delete a node while hovering over it with the mouse
Press 1 to summon a node
Press Z to snap the nodes to grid