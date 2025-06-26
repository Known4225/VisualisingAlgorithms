#include "../include/turtle.h"
#include <time.h>

/*
Bresenham's algorithms
Line algorithm
Circle algorithm
*/

typedef struct {
    char keys[5]; // for keybinds
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

    int operations;
    int octant;

    int deltaX;
    int deltaY;
    int trackX;
    int trackY;
    int error;

    char leftRight;
    char topBottom;
    char evenOdd;
    char horizVert;
    

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
    for (int i = 0; i < 5; i++) {
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
    
    self.octant = 0;
    self.operations = 0;

    /* bresenham components */
    self.deltaX = 0;
    self.deltaY = 0;
    self.trackX = 0;
    self.trackY = 0;
    self.error = 0;

    self.leftRight = 0;
    self.topBottom = 0;
    self.horizVert = 0;
    

    self.pixels = list_init();
    *selfp = self;
}

void bresenhamStep(Line *selfp) {
    Line self = *selfp;
    if (self.operations == 0) {
        /* first step */
        double slope = (double) (self.pixel2[1] - self.pixel1[1]) / (self.pixel2[0] - self.pixel1[0]);

        if (self.pixel1[0] > self.pixel2[0] || self.pixel1[0] == self.pixel2[0] && self.pixel1[1] > self.pixel2[1]) {
            /* left */
            self.leftRight = 0;
            self.deltaX = (self.pixel1[0] - self.pixel2[0]);
        } else {
            /* right */
            self.leftRight = 1;
            self.deltaX = (self.pixel2[0] - self.pixel1[0]);
            
        }
        if (self.pixel1[1] > self.pixel2[1]) {
            /* bottom */
            self.topBottom = 1;
            self.deltaY = (self.pixel1[1] - self.pixel2[1]);
        } else {
            /* top */
            self.topBottom = 0;
            self.deltaY = (self.pixel2[1] - self.pixel1[1]);
        }

        if (fabs(slope) > 1) {
            /* vertically aligned */
            self.horizVert = 1;
        } else {
            /* horizontally aligned */
            self.horizVert = 0;
        }

        
        // if (self.topBottom) {
        //     if (self.leftRight) {
        //         /* bottom right */
        //         if (fabs(slope) > 1) {
        //             self.octant = 6;
        //             self.evenOdd = 0;
        //             self.horizVert = 1;
        //         } else {
        //             self.octant = 7;
        //             self.evenOdd = 1;
        //             self.horizVert = 0;
        //         }
        //     } else {
        //         /* bottom left */
        //         if (fabs(slope) > 1) {
        //             self.octant = 5;
        //             self.evenOdd = 1;
        //             self.horizVert = 1;
        //         } else {
        //             self.octant = 4;
        //             self.evenOdd = 0;
        //             self.horizVert = 0;
        //         }
        //     }
        // } else {
        //     if (self.leftRight) {
        //         /* top right */
        //         if (fabs(slope) > 1) {
        //             self.octant = 1;
        //             self.evenOdd = 1;
        //             self.horizVert = 1;
        //         } else {
        //             self.octant = 0;
        //             self.evenOdd = 0;
        //             self.horizVert = 0;
        //         }
        //     } else {
        //         /* top left */
        //         if (fabs(slope) > 1) {
        //             self.octant = 2;
        //             self.evenOdd = 0;
        //             self.horizVert = 1;
        //         } else {
        //             self.octant = 3;
        //             self.evenOdd = 1;
        //             self.horizVert = 0;
        //         }
        //     }
        // }
        // printf("octant: %d\n", self.octant);

        self.trackX = self.pixel1[0];
        self.trackY = self.pixel1[1];
        self.error = 0;
        
    } else {
        if (self.horizVert) {
            if ((self.topBottom && self.trackY >= self.pixel2[1]) || (!self.topBottom) && self.trackY <= self.pixel2[1]) {
                list_append(self.pixels, (unitype) self.trackX, 'i');
                list_append(self.pixels, (unitype) self.trackY, 'i');
                self.error += self.deltaX;
                if ((self.error << 1) >= self.deltaY) {
                    if (self.leftRight)
                        self.trackX++;
                    else
                        self.trackX--;
                    self.error -= self.deltaY;
                }
                if (self.topBottom)
                    self.trackY--;
                else
                    self.trackY++;
            }
        } else {
            if ((!self.leftRight && self.trackX >= self.pixel2[0]) || (self.leftRight) && self.trackX <= self.pixel2[0]) {
                list_append(self.pixels, (unitype) self.trackX, 'i');
                list_append(self.pixels, (unitype) self.trackY, 'i');
                self.error += self.deltaY;
                if ((self.error << 1) >= self.deltaX) {
                    if (self.topBottom)
                        self.trackY--;
                    else
                        self.trackY++;
                    self.error -= self.deltaX;
                }
                if (self.leftRight)
                    self.trackX++;
                else
                    self.trackX--;
            }
        }
    }
    *selfp = self;
}

void instantLine(Line *selfp, int x1, int y1, int x2, int y2) {
    Line self = *selfp;
    /* first step */
    double slope = (double) (y2 - y1) / (x2 - x1);
    int deltaX;
    int deltaY;
    char leftRight;
    char topBottom;
    char horizVert;
    if (x1 > x2 || x1 == x2 && y1 > y2) {
        /* left */
        leftRight = 0;
        deltaX = (x1 - x2);
    } else {
        /* right */
        leftRight = 1;
        deltaX = (x2 - x1);
        
    }
    if (y1 > y2) {
        /* bottom */
        topBottom = 1;
        deltaY = (y1 - y2);
    } else {
        /* top */
        topBottom = 0;
        deltaY = (y2 - y1);
    }

    if (fabs(slope) > 1) {
        /* vertically aligned */
        horizVert = 1;
    } else {
        /* horizontally aligned */
        horizVert = 0;
    }

    int trackX = x1;
    int trackY = y1;
    int error = 0;
    

    if (horizVert) {
        while ((topBottom && trackY >= y2) || (!topBottom) && trackY <= y2) {
            list_append(self.pixels, (unitype) trackX, 'i');
            list_append(self.pixels, (unitype) trackY, 'i');
            error += deltaX;
            if ((error << 1) >= deltaY) {
                if (leftRight)
                    trackX++;
                else
                    trackX--;
                error -= deltaY;
            }
            if (topBottom)
                trackY--;
            else
                trackY++;
        }
    } else {
        while ((!leftRight && trackX >= x2) || (leftRight) && trackX <= x2) {
            list_append(self.pixels, (unitype) trackX, 'i');
            list_append(self.pixels, (unitype) trackY, 'i');
            error += deltaY;
            if ((error << 1) >= deltaX) {
                if (topBottom)
                    trackY--;
                else
                    trackY++;
                error -= deltaX;
            }
            if (leftRight)
                trackX++;
            else
                trackX--;
        }
    }
    *selfp = self;
}

void renderGrid(Line *selfp) { // renders the grid
    Line self = *selfp;
    // printf("%lf\n", (640 * self.screenSize / self.gridSize));
    /* the point 0, 0 is actually the position at (self.screenX) * self.screenSize
    so if we want our lines to cross (0, 0) we need it to go to self.screenX * self.screenSize and then move toward the visual screen in self.gridSize * self.screenSize steps
    the number of steps it will take is equal to floor((self.screenX * self.screenSize) / (self.screenSize * self.gridSize)), or floor(self.screenX / self.gridSize)
    then it will move an additional floor(320 / (self.screenSize * self.gridSize))
    */
    turtlePenColor(30, 30, 30);
    turtlePenSize(self.screenSize);
    double startX = self.screenSize * (self.screenX - self.gridSize * (floor(self.screenX / self.gridSize) + 1 + floor(320 / (self.screenSize * self.gridSize))));
    double startY = self.screenSize * (self.screenY - self.gridSize * (floor(self.screenY / self.gridSize) - floor(180 / (self.screenSize * self.gridSize))));
    double x = startX;
    double y = startY;
    for (int i = -1; i < (640 / (self.screenSize * self.gridSize)); i++) {
        turtleGoto(x, 180);
        turtlePenDown();
        turtleGoto(x, -180);
        turtlePenUp();
        x += self.screenSize * self.gridSize;
    }
    for (int i = -1; i < (360 / (self.screenSize * self.gridSize)); i++) {
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
    double correctionX = 0;
    double correctionY = 0;
    if (self.pixel1[0] != 2147483647) {
        turtleQuad((self.pixel1[0] * self.gridSize + self.screenX) * self.screenSize, (self.pixel1[1] * self.gridSize + self.screenY) * self.screenSize, 
        (self.pixel1[0] * self.gridSize + self.gridSize + self.screenX) * self.screenSize, (self.pixel1[1] * self.gridSize + self.screenY) * self.screenSize, 
        (self.pixel1[0] * self.gridSize + self.gridSize + self.screenX) * self.screenSize, (self.pixel1[1] * self.gridSize + self.gridSize + self.screenY) * self.screenSize,
        (self.pixel1[0] * self.gridSize + self.screenX) * self.screenSize, (self.pixel1[1] * self.gridSize + self.gridSize + self.screenY) * self.screenSize,
        50, 50, 50, 0);
    }
    if (self.pixel2[0] != 2147483647) {
        turtleQuad((self.pixel2[0] * self.gridSize + self.screenX) * self.screenSize, (self.pixel2[1] * self.gridSize + self.screenY) * self.screenSize, 
        (self.pixel2[0] * self.gridSize + self.gridSize + self.screenX) * self.screenSize, (self.pixel2[1] * self.gridSize + self.screenY) * self.screenSize, 
        (self.pixel2[0] * self.gridSize + self.gridSize + self.screenX) * self.screenSize, (self.pixel2[1] * self.gridSize + self.gridSize + self.screenY) * self.screenSize,
        (self.pixel2[0] * self.gridSize + self.screenX) * self.screenSize, (self.pixel2[1] * self.gridSize + self.gridSize + self.screenY) * self.screenSize,
        50, 50, 50, 0);
    }
    for (int i = 0; i < self.pixels -> length; i += 2) {
        turtleQuad((self.pixels -> data[i].i * self.gridSize + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i * self.gridSize + self.screenY) * self.screenSize, 
        (self.pixels -> data[i].i * self.gridSize + self.gridSize + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i * self.gridSize + self.screenY) * self.screenSize, 
        (self.pixels -> data[i].i * self.gridSize + self.gridSize + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i * self.gridSize + self.gridSize + self.screenY) * self.screenSize,
        (self.pixels -> data[i].i * self.gridSize + self.screenX) * self.screenSize, (self.pixels -> data[i + 1].i * self.gridSize + self.gridSize + self.screenY) * self.screenSize,
        0, 0, 0, 0);
    }
    if (self.operations > 0) {
        turtlePenColor(0, 0, 0);
        turtlePenSize(3 * self.screenSize);
        turtleGoto((self.pixel1[0] * self.gridSize + self.gridSize / 2 + self.screenX) * self.screenSize, (self.pixel1[1] * self.gridSize + self.gridSize / 2 + self.screenY) * self.screenSize);
        turtlePenDown();
        turtleGoto((self.pixel2[0] * self.gridSize + self.gridSize / 2 + self.screenX) * self.screenSize, (self.pixel2[1] * self.gridSize + self.gridSize / 2 + self.screenY) * self.screenSize);
        turtlePenUp();
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
        if (self.keys[0] == 1) {
            self.keys[0] = 0;
            if (fabs(self.mouseX - self.focalX) < 0.01 && fabs(self.mouseY - self.focalY) < 0.01) {
                if (self.toggle) {
                    int x = floor(((self.mouseX / self.screenSize - self.screenX) / self.gridSize));
                    int y = floor(((self.mouseY / self.screenSize - self.screenY) / self.gridSize));
                    if (x != self.pixel2[0] || y != self.pixel2[1]) {
                        list_clear(self.pixels);
                        self.pixel2[0] = x;
                        self.pixel2[1] = y;
                        self.toggle = 0;
                        self.operations = 0;
                    }
                } else {
                    int x = floor(((self.mouseX / self.screenSize - self.screenX) / self.gridSize));
                    int y = floor(((self.mouseY / self.screenSize - self.screenY) / self.gridSize));
                    if (x != self.pixel1[0] || y != self.pixel1[1]) {
                        list_clear(self.pixels);
                        self.pixel1[0] = x;
                        self.pixel1[1] = y;
                        self.toggle = 1;
                        self.operations = 0;
                    }
                }
            }
        }
    }
    *selfp = self;
}

void scrollTick(Line *selfp) {
    Line self = *selfp;
    double mouseWheel = turtleMouseWheel(); // behavior is a bit different for the scroll wheel
    if (mouseWheel > 0) {
        /* zoom in */
        self.screenX -= (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.screenSize;
        self.screenY -= (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.screenSize;
        self.screenSize *= self.scrollSpeed;
    }
    if (mouseWheel < 0) {
        /* zoom out */
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
            if (self.pixel1[0] != 2147483647 && self.pixel2[0] != 2147483647) {
                bresenhamStep(&self);
                self.operations += 1;
            }
        } else {
            self.keys[2] += 1;
            if (self.keys[2] > 30) {
                self.keys[1] = 0;
                self.keys[2] = 31;
            }
        }
    } else {
        self.keys[1] = 0;
        self.keys[2] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_C)) {
        if (self.keys[3] == 0) {
            self.keys[3] = 1;
            list_clear(self.pixels);
            for (int i = 0; i < 2; i++) {
                self.pixel1[i] = 2147483647;
                self.pixel2[i] = 2147483647;
            }
            self.toggle = 0;
        }
    } else {
        self.keys[3] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_A)) {
        if (self.keys[4] == 0) {
            self.keys[4] = 1;
        }
        if (self.pixel1[0] == 2147483647) {
            self.pixel1[0] = 0;
            self.pixel1[1] = 0;
        }
        // self.pixel2[0] = 2147483647;
        // self.pixel2[1] = 2147483647;
        self.toggle = 1;
        self.operations = 0;
        list_clear(self.pixels);
        instantLine(&self, self.pixel1[0], self.pixel1[1], floor(((self.mouseX / self.screenSize - self.screenX) / self.gridSize)), floor(((self.mouseY / self.screenSize - self.screenY) / self.gridSize)));
    } else {
        if (self.keys[4] == 1) {
            list_clear(self.pixels);
            self.keys[4] = 0;
        }
    }
    *selfp = self;
}

int main(int argc, char *argv[]) {
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    const GLFWvidmode *monitorSize = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int32_t windowHeight = monitorSize -> height * 0.85;
    GLFWwindow *window = glfwCreateWindow(windowHeight * 16 / 9, windowHeight, "Bresenham Line", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, windowHeight * 16 / 9, windowHeight, windowHeight * 16 / 9, windowHeight);

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