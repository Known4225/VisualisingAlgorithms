This is a visualisation of the Bresenham line and circle algorithms.

Windows:
gcc BresenhamLine.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o BresenhamLine.exe

BresenhamLine.exe

Linux:
gcc BresenhamLine.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o BresenhamLine.o

./BresenhamLine.o

Keybinds:
Click and drag to move around the screen, use the scroll wheel to zoom
Click on a pixel to set it as an end point
Use space to advance the animation (must have both endpoints set)
Press C to clear end points
Hold A to make a line from the first endpoint to the mouse position