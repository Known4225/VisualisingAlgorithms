#include "include/textGL.h"
#include <time.h>

/* general design plan:
Heapsort visualisation
Adapted from my python project
https://github.com/Known4225/Heapsort

*/

typedef struct {
    char keys[5];
    char phase;
    int operations;
    list_t *unsorted;
    int unsortHighlight;
    list_t *maxHeap;
    list_t *sorted;
    double mouseX;
    double mouseY;
    double focalX;
    double focalY;
    double focalCSX;
    double focalCSY;
    double screenX;
    double screenY;
    double screenSize;
    double scrollSpeed;
    double arraySegmentSize;
    double nodeSize;
} Heapsort;

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

void init(Heapsort *selfp, int length) {
    Heapsort self = *selfp;
    self.unsorted = list_init();
    for (int i = 0; i < length; i++) {
        list_append(self.unsorted, (unitype) randomInt(0, 5 * length), 'i');
    }
    for (int i = 0; i < 5; i++) {
        self.keys[i] = 0;
    }
    self.unsortHighlight = -1;
    self.phase = 0;
    self.arraySegmentSize = 20;
    self.nodeSize = 40;
    self.operations = 0;
    self.mouseX = 0;
    self.mouseY = 0;
    self.screenX = 0;
    self.screenY = 0;
    self.screenSize = 1;
    self.focalX = 0;
    self.focalY = 0;
    self.focalCSX = 0;
    self.focalCSY = 0;
    self.scrollSpeed = 1.15;
    *selfp = self;
}

void createMaxHeap(Heapsort *selfp) { // creates the max heap
    Heapsort self = *selfp;
    // printf("%d %d\n", self.unsortHighlight, self.unsorted -> length);
    if (self.unsortHighlight + 1 <= self.unsorted -> length) { // potential glitch: -1 is not less than 15 if the 15 is an unsigned int, cuz -1 is interpreted as the max int value, even though its type is a signed int
        self.unsortHighlight += 1;
        list_append(self.maxHeap, self.unsorted -> data[self.unsortHighlight], 'i');
    }

    *selfp = self;
}

void heapify(Heapsort *selfp) { // Heapify algorithm sorts the list via the max heap
    Heapsort self = *selfp;
    for (int i = 0; i < self.maxHeap -> length; i++) {
        
    }
    *selfp = self;
}

void renderHeap(Heapsort *selfp) {
    Heapsort self = *selfp;
    
    *selfp = self;
}

void renderArray(Heapsort *selfp) {
    Heapsort self = *selfp;

    /* unsorted array */
    turtlePenColor(0, 0, 0);
    turtlePenSize(2 * self.screenSize);
    for (int i = 0; i < self.unsorted -> length; i++) {
        int xpos = (i - self.unsorted -> length / 2.0) * self.arraySegmentSize;
        if (self.unsortHighlight == i) {
            turtleQuad((xpos - self.arraySegmentSize / 2 + self.screenX), (160 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize / 2 + self.screenX), (160 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize / 2 + self.screenX), (140 + self.screenY) * self.screenSize,
            (xpos - self.arraySegmentSize / 2 + self.screenX), (140 + self.screenY) * self.screenSize,
            19, 236, 48, 0);
        }
        turtleGoto((xpos - self.arraySegmentSize / 2 + self.screenX) * self.screenSize, (160 + self.screenY) * self.screenSize);
        turtlePenDown();
        turtleGoto((xpos - self.arraySegmentSize / 2 + self.screenX) * self.screenSize, (140 + self.screenY) * self.screenSize);
        turtlePenUp();
        char num[12];
        sprintf(num, "%d", self.unsorted -> data[i].i);
        textGLWriteString(num, (xpos + self.screenX) * self.screenSize, (150 + self.screenY) * self.screenSize, self.screenSize * self.arraySegmentSize * 0.5, 50);
        sprintf(num, "%d", i + 1);
        textGLWriteString(num, (xpos + self.screenX) * self.screenSize, (167 + self.screenY) * self.screenSize, self.screenSize * self.arraySegmentSize * 0.3, 50);
    }
    turtleGoto(((self.unsorted -> length / -2.0 - 0.5) * self.arraySegmentSize + self.screenX) * self.screenSize, (160 + self.screenY) * self.screenSize);
    turtlePenDown();
    turtleGoto(((self.unsorted -> length / 2.0 - 0.5) * self.arraySegmentSize + self.screenX) * self.screenSize, (160 + self.screenY) * self.screenSize);
    turtleGoto(((self.unsorted -> length / 2.0 - 0.5) * self.arraySegmentSize + self.screenX) * self.screenSize, (140 + self.screenY) * self.screenSize);
    turtleGoto(((self.unsorted -> length / -2.0 - 0.5) * self.arraySegmentSize + self.screenX) * self.screenSize, (140 + self.screenY) * self.screenSize);
    turtlePenUp();

    /* max heap */

    *selfp = self;
}

void mouseTick(Heapsort *selfp) {
    Heapsort self = *selfp;
    turtleGetMouseCoords(); // get the mouse coordinates (turtle.mouseX, turtle.mouseY)
    self.mouseX = turtle.mouseX;
    self.mouseY = turtle.mouseY;
    if (turtleMouseDown()) {
        if (self.keys[0] == 0) {
            self.keys[0] = 1;
            self.focalX = self.mouseX;
            self.focalY = self.mouseY;
            self.focalCSX = self.screenX;
            self.focalCSY = self.screenY;
        } else {
            self.screenX = (self.mouseX - self.focalX) / self.screenSize + self.focalCSX;
            self.screenY = (self.mouseY - self.focalY) / self.screenSize + self.focalCSY;
        }
    } else {
        self.keys[0] = 0;
    }
    *selfp = self;
}

void scrollTick(Heapsort *selfp) {
    Heapsort self = *selfp;
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

void hotkeyTick(Heapsort *selfp) {
    Heapsort self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE)) { // space (dummy)
        if (self.keys[1] == 0) {
            self.keys[1] = 1;
            if (self.phase) {
                heapify(&self);
            } else {
                createMaxHeap(&self);
            }
        }
    } else {
        self.keys[1] = 0;
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
    window = glfwCreateWindow(1280, 720, "Heapsort", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, 1280, 720);

    /* initialize turtle */
    turtleInit(window, -320, -180, 320, 180);
    /* initialise textGL */
    textGLInit(window, "include/fontBez.tgl");

    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;

    turtleBgColor(150, 150, 150);
    Heapsort obj; // principle object
    srand(time(NULL)); // randomiser init seed
    if (argc == 1) {
        init(&obj, 15);
    } else {
        int listLength = 0;
        sscanf(argv[1], "%d\n", &listLength);
        init(&obj, listLength);
    }
    
    while (turtle.close == 0) { // main loop
        start = clock();
        turtleClear();
        mouseTick(&obj);
        scrollTick(&obj);
        hotkeyTick(&obj);
        renderArray(&obj);
        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}