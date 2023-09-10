All sorts of sorts!

Windows:
gcc Heapsort.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o Heapsort.exe

Heapsort.exe

Linux:
gcc Heapsort.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o Heapsort.o

./Heapsort.o

Keybinds:
Press space to advance animation
Press R to reset
When you run the program you can add a number at the end to specify the length of the list to sort