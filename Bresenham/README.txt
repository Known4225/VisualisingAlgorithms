This is a visualisation of the Bresenham line and circle algorithms.

Windows:
gcc BresenhamLine.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o BresenhamLine.exe

BresenhamLine.exe


Linux:
gcc BresenhamLine.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o BresenhamLine.o

./BresenhamLine.o

Keybinds:
Click to create end points
press space to advance animation