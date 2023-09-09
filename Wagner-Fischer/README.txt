Visual Demonstration of the Wagner-Fischer algorithm for Levenshtein edit distance.

Windows:
gcc Wagner-Fischer.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o Wagner-Fischer.exe

Wagner-Fischer.exe {string1} {string2}

Use the mouse to move around
Use space to step through the animation

Linux:
gcc Wagner-Fischer.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Wagner-Fischer.o

./Wagner-Fischer.o {string1} {string2}