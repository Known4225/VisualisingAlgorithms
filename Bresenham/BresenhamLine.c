#include "include/turtle.h"
#include <time.h>

/*
Bresenham's algorithms

*/

typedef struct {
    char keys[4]; // for keybinds
    char toggle; // for placing first and second points
    int pixel1[2]; // x, y coordinates for pixel1 (starting pixel) on the grid
    int pixel2[2];
    double gridSize; // for grid
    double screenX;
    double screenY;
    double screenSize;
    double scrollSpeed;

    double mouseX; // mouse position
    double mouseY;
    double focalX;
    double focalY;
    double focalCSX;
    double focalCSY;

    list_t *pixels;
} Line;

extern inline int randomInt(int lowerBound, int upperBound) { // random integer between lower and upper bound (inclusive)
    return (rand() % (upperBound - lowerBound + 1) + lowerBound);
}

extern inline double randomDouble(double lowerBound, double upperBound) { // random double between lower and upper bound
    return (rand() * (upperBound - lowerBound) / RAND_MAX + lowerBound); // probably works idk
}

extern inline double dmod(double a, double modulus) { // always positive mod
    double out = fmod(a, modulus);
    if (out < 0) {
        return modulus + out;
    }
    return out;
}

void init(Line *selfp, double gridSize) {
    Line self = *selfp;
    for (int i = 0; i < 4; i++) {
        self.keys[i] = 0;
    }
    for (int i = 0; i < 2; i++) {
        self.pixel1[i] = 2147483647; // these values are reserved for NOT_SET
        self.pixel2[i] = 2147483647;
    }
    self.toggle = 0;
    self.gridSize = gridSize;
    self.screenX = 0;
    self.screenY = 0;
    self.screenSize = 1;
    self.scrollSpeed = 1.15;

    self.mouseX = 0;
    self.mouseY = 0;
    self.focalX = 0;
    self.focalY = 0;
    self.focalCSX = 0;
    self.focalCSY = 0;

    self.pixels = list_init();
    *selfp = self;
}

void renderGrid(Line *selfp) { // renders the grid
    Line self = *selfp;
    // printf("%lf\n", (640 * self.screenSize / self.gridSize));
    turtlePenColor(30, 30, 30);
    turtlePenSize(self.screenSize * 2);
    double x = -320 - dmod(self.screenX, self.gridSize);
    double y = 180 - dmod(self.screenY, self.gridSize);
    for (int i = 0; i < (640 * self.screenSize / self.gridSize); i++) {
        turtleGoto(x, 180);
        turtlePenDown();
        turtleGoto(x, -180);
        turtlePenUp();
        x += self.screenSize * self.gridSize;
    }
    for (int i = 0; i < (360 * self.screenSize / self.gridSize); i++) {
        turtleGoto(-320, y);
        turtlePenDown();
        turtleGoto(320, y);
        turtlePenUp();
        y -= self.screenSize * self.gridSize;
    }
    *selfp = self;
}

void renderPixels(Line *selfp) { // renders all coloured in pixels
    Line self = *selfp;
    if (self.pixel1[0] != 2147483647) {
        turtleQuad((self.pixel1[0] + self.screenX) * self.screenSize, (self.pixel1[1] + self.screenY) * self.screenSize, 
        (self.pixel1[0] + self.gridSize + self.screenX) * self.screenSize, (self.pixel1[1] + self.screenY) * self.screenSize, 
        (self.pixel1[0] + self.gridSize + self.screenX) * self.screenSize, (self.pixel1[1] + self.gridSize + self.screenY) * self.screenSize,
        (self.pixel1[0] + self.screenX) * self.screenSize, (self.pixel1[1] + self.gridSize + self.screenY) * self.screenSize,
        0, 0, 0, 0);
    }
    if (self.pixel2[0] != 2147483647) {
        turtleQuad((self.pixel2[0] + self.screenX) * self.screenSize, (self.pixel2[1] + self.screenY) * self.screenSize, 
        (self.pixel2[0] + self.gridSize + self.screenX) * self.screenSize, (self.pixel2[1] + self.screenY) * self.screenSize, 
        (self.pixel2[0] + self.gridSize + self.screenX) * self.screenSize, (self.pixel2[1] + self.gridSize + self.screenY) * self.screenSize,
        (self.pixel2[0] + self.screenX) * self.screenSize, (self.pixel2[1] + self.gridSize + self.screenY) * self.screenSize,
        0, 0, 0, 0);
    }
    for (int i = 0; i < self.pixels -> length; i += 2) {
        turtleQuad((self.pixels -> data[i].i + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i + self.screenY) * self.screenSize, 
        (self.pixels -> data[i].i + self.gridSize + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i + self.screenY) * self.screenSize, 
        (self.pixels -> data[i].i + self.gridSize + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i + self.gridSize + self.screenY) * self.screenSize,
        (self.pixels -> data[i].i + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i + self.gridSize + self.screenY) * self.screenSize,
        0, 0, 0, 0);
    }
    *selfp = self;
}


void mouseTick(Line *selfp) {
    Line self = *selfp;
    turtleGetMouseCoords(); // get the mouse coordinates (turtle.mouseX, turtle.mouseY)
    self.mouseX = turtle.mouseX;
    self.mouseY = turtle.mouseY;
    
    if (turtleMouseDown()) {
        if (self.keys[0] == 0) {
            self.keys[0] = 1;
            /* first tick of mouse click */
            if (self.toggle) {
                self.pixel1[0] = floor((self.mouseX + self.screenX) * self.screenSize / self.gridSize);
                self.pixel1[1] = floor((self.mouseY + self.screenY) * self.screenSize / self.gridSize);
            } else {
                self.pixel2[0] = floor((self.mouseX + self.screenX) * self.screenSize / self.gridSize);
                self.pixel2[1] = floor((self.mouseY + self.screenY) * self.screenSize / self.gridSize);
            }

            self.focalX = self.mouseX;
            self.focalY = self.mouseY;
            self.focalCSX = self.screenX;
            self.focalCSY = self.screenY;

        } else {
            /* mouse down */
            self.screenX = (self.mouseX - self.focalX) / self.screenSize + self.focalCSX;
            self.screenY = (self.mouseY - self.focalY) / self.screenSize + self.focalCSY;
        }
    } else {
        /* mouse is not pressed */
        if (self.keys == 1) {
            self.keys[0] = 0;
            if (fabs(self.mouseX - self.focalX) < 0.01 && fabs(self.mouseY - self.focalY) < 0.01) {

            }
        }
    }
    *selfp = self;
}

void scrollTick(Line *selfp) {
    Line self = *selfp;
    double mouseWheel = turtleMouseWheel(); // behavior is a bit different for the scroll wheel
    if (mouseWheel > 0) {
        self.screenX -= (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.screenSize;
        self.screenY -= (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.screenSize;
        self.screenSize *= self.scrollSpeed;
    }
    if (mouseWheel < 0) {
        self.screenSize /= self.scrollSpeed;
        self.screenX += (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.screenSize;
        self.screenY += (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.screenSize;
    }
    *selfp = self;
}

void hotkeyTick(Line *selfp) {
    Line self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE)) {
        if (self.keys[1] == 0) {
            self.keys[1] = 1;
        }
    } else {
        self.keys[1] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_C)) {
        if (self.keys[2] == 0) {
            self.keys[2] = 1;
            list_clear(self.pixels);
            for (int i = 0; i < 2; i++) {
                self.pixel1[i] = 2147483647;
                self.pixel2[i] = 2147483647;
            }
        }
    } else {
        self.keys[2] = 0;
    }
    *selfp = self;
}

int main(int argc, char *argv[]) {
    GLFWwindow* window;
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Bresenham Line", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, 1280, 720);

    /* initialize turtle */
    turtleInit(window, -320, -180, 320, 180);
    /* initialise textGL */
    //textGLInit(window, "include/fontBez.tgl");

    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;

    turtleBgColor(180, 180, 180);
    Line obj; // principle object
    init(&obj, 30);
    srand(time(NULL)); // randomiser init seed
    
    while (turtle.close == 0) { // main loop
        start = clock();
        turtleClear();
        mouseTick(&obj);
        scrollTick(&obj);
        hotkeyTick(&obj);
        renderGrid(&obj);
        renderPixels(&obj);

        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}