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
    list_t *toSort;
    int loaded;
    int highlight;
    int highlightCyan;
    int treeHighlight;
    int treeHighlightCyan;
    int sorted;
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
    double maximumAngle;
    double angleChange;
    double barScale;
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
    self.toSort = list_init();
    for (int i = 0; i < length; i++) {
        list_append(self.toSort, (unitype) randomInt(0, 5 * length), 'i');
    }
    for (int i = 0; i < 5; i++) {
        self.keys[i] = 0;
    }
    self.loaded = 0;
    self.highlight = -1;
    self.highlightCyan = length;
    self.treeHighlight = -1;
    self.treeHighlightCyan = -1;
    self.sorted = length - 1;
    self.phase = 0;
    self.arraySegmentSize = 20;
    self.nodeSize = 30;
    self.maximumAngle = 30;
    self.angleChange = 2;

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

    self.barScale = 5;
    *selfp = self;
}

void createunsorted(Heapsort *selfp) { // creates the max heap
    Heapsort self = *selfp;
    // printf("%d %d\n", self.highlight, self.toSort -> length);
    if (self.treeHighlight > 0 && self.toSort -> data[self.treeHighlight].d > self.toSort -> data[(self.treeHighlight + 1) / 2 - 1].d) {
        unitype temp = self.toSort -> data[self.treeHighlight];
        self.toSort -> data[self.treeHighlight] = self.toSort -> data[(self.treeHighlight + 1) / 2 - 1];
        self.toSort -> data[(self.treeHighlight + 1) / 2 - 1] = temp;
        self.treeHighlight = (self.treeHighlight + 1) / 2 - 1;
    } else {
        if (self.highlight + 1 <= self.toSort -> length) { // potential glitch: -1 is not less than 15 if the 15 is an unsigned int, cuz -1 is interpreted as the max int value, even though its type is a signed int
            self.highlight += 1;
            self.treeHighlight = self.highlight;
            if (self.highlight + 1 <= self.toSort -> length)
                self.loaded += 1;
            else
                self.phase += 1;
        }
    }

    *selfp = self;
}

void heapify(Heapsort *selfp) { // Heapify algorithm sorts the list via the max heap
    Heapsort self = *selfp;
    if (self.phase > 2) {
        if ((self.treeHighlight + 1) * 2 - 1 < self.sorted && (self.toSort -> data[self.treeHighlight].d < self.toSort -> data[(self.treeHighlight + 1) * 2 - 1].d || self.toSort -> data[self.treeHighlight].d < self.toSort -> data[(self.treeHighlight + 1) * 2].d)) {
            if (self.toSort -> data[(self.treeHighlight + 1) * 2 - 1].d > self.toSort -> data[(self.treeHighlight + 1) * 2].d) {
                unitype temp = self.toSort -> data[self.treeHighlight];
                self.toSort -> data[self.treeHighlight] = self.toSort -> data[(self.treeHighlight + 1) * 2 - 1];
                self.toSort -> data[(self.treeHighlight + 1) * 2 - 1] = temp;
                self.treeHighlight = (self.treeHighlight + 1) * 2 - 1;
            } else {
                unitype temp = self.toSort -> data[self.treeHighlight];
                self.toSort -> data[self.treeHighlight] = self.toSort -> data[(self.treeHighlight + 1) * 2];
                self.toSort -> data[(self.treeHighlight + 1) * 2] = temp;
                self.treeHighlight = (self.treeHighlight + 1) * 2;
            }
        } else if ((self.treeHighlight + 1) * 2 - 1 == self.sorted && self.toSort -> data[self.treeHighlight].d < self.toSort -> data[(self.treeHighlight + 1) * 2 - 1].d) { // special case: only left node is available and is larger than parent
            unitype temp = self.toSort -> data[self.treeHighlight];
            self.toSort -> data[self.treeHighlight] = self.toSort -> data[(self.treeHighlight + 1) * 2 - 1];
            self.toSort -> data[(self.treeHighlight + 1) * 2 - 1] = temp;
            self.treeHighlight = (self.treeHighlight + 1) * 2 - 1;
        } else {
            self.phase = 1;
            heapify(&self);
        }
    } else if (self.phase > 1) {
        unitype temp = self.toSort -> data[self.sorted];
        self.toSort -> data[self.sorted] = self.toSort -> data[0];
        self.toSort -> data[0] = temp;
        self.treeHighlight = 0;
        self.treeHighlightCyan = self.sorted;
        self.highlightCyan = self.sorted;
        self.sorted -= 1;
        self.phase += 1;
    } else {
        self.treeHighlightCyan = 0;
        self.treeHighlight = self.sorted;
        if (self.sorted == 0) {
            self.highlightCyan = 0;
            self.treeHighlight = -1;
            self.treeHighlightCyan = -1;
        } else {
            self.phase += 1;
        }
    }
    *selfp = self;
}

void renderHeap(Heapsort *selfp) {
    Heapsort self = *selfp;
    list_t *xposList = list_init();
    list_append(xposList, (unitype) 0.0, 'd'); // dummy item for 0th
    int depth = ceil(log(self.loaded + 1) / log(2)); // depth of the tree
    double currentAngle = self.maximumAngle / pow(self.angleChange, depth - 1);
    int oldLog = 0;
    double ypos = 90;
    for (int i = 0; i < self.loaded; i++) { // first loop: render connections
        int log2 = floor(log(i + 1) / log(2));
        // double xpos = (pow(2, log2) - 1) * self.nodeSize * -1 + (self.nodeSize * 2 * (i + 1 - pow(2, log2))); // naive approach
        if (log2 > oldLog) {
            oldLog = log2;
            currentAngle *= self.angleChange;
            ypos -= self.nodeSize * 2;
        }
        double xpos;
        if (i == 0) {
            xpos = 0;
        } else {
            turtleGoto((xposList -> data[(i + 1) / 2].d + self.screenX) * self.screenSize, (ypos + self.nodeSize * 2 + self.screenY) * self.screenSize);
            turtlePenColor(0, 0, 0);
            turtlePenSize(2 * self.screenSize);
            turtlePenDown();
            xpos = xposList -> data[(i + 1) / 2].d; // tapered angle approach
            if (i % 2) {
                xpos -= (self.nodeSize * 0.4) / (tan(currentAngle / 57.2958));
            } else {
                xpos += (self.nodeSize * 0.4) / (tan(currentAngle / 57.2958));
            }
            turtleGoto((xpos + self.screenX) * self.screenSize, (ypos + self.screenY) * self.screenSize);
            turtlePenUp();
        }
        list_append(xposList, (unitype) xpos, 'd');
    }
    oldLog = 0;
    ypos = 90;
    for (int i = 0; i < self.loaded; i++) { // second loop: render nodes
        int log2 = floor(log(i + 1) / log(2));
        if (log2 > oldLog) {
            oldLog = log2;
            ypos -= self.nodeSize * 2;
        }
        turtleGoto((xposList -> data[i + 1].d + self.screenX) * self.screenSize, (ypos + self.screenY) * self.screenSize);
        turtlePenColor(0, 0, 0);
        turtlePenSize(self.nodeSize * self.screenSize);
        turtlePenDown();
        turtlePenUp();
        turtlePenSize(self.nodeSize * self.screenSize * 0.9);
        if (self.treeHighlight == i) {
            turtlePenColor(19, 236, 48);
        } else if (self.treeHighlightCyan == i) {
            turtlePenColor(43, 188, 255);
        } else if (self.highlightCyan <= i) {
            turtlePenColor(109, 209, 255);
        } else {
            turtlePenColor(120, 120, 120);
        }
        turtlePenDown();
        turtlePenUp();
        turtlePenColor(0, 0, 0);
        char num[12];
        sprintf(num, "%d", self.toSort -> data[i].i);
        textGLWriteString(num, (xposList -> data[i + 1].d + self.screenX) * self.screenSize, (ypos + self.screenY) * self.screenSize, self.nodeSize * self.screenSize * 0.4, 50);
    }
    list_free(xposList);
    *selfp = self;
}

void renderArray(Heapsort *selfp) {
    Heapsort self = *selfp;
    double ypos = 125;
    double xpos = (self.toSort -> length / -2.0) * self.arraySegmentSize;
    turtlePenColor(0, 0, 0);
    turtlePenSize(2 * self.screenSize);
    for (int i = 0; i < self.toSort -> length; i++) {
        if (self.treeHighlight == i) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            19, 236, 48, 0);
        }
        if (self.highlightCyan <= i) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            43, 188, 255, 0);
        }
        turtleGoto((xpos + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize);
        turtlePenDown();
        turtleGoto((xpos + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize);
        turtlePenUp();
        char num[12];
        sprintf(num, "%d", self.toSort -> data[i].i);
        textGLWriteString(num, (xpos + self.arraySegmentSize / 2 + self.screenX) * self.screenSize, (ypos + self.screenY) * self.screenSize, self.screenSize * self.arraySegmentSize * 0.5, 50);
        sprintf(num, "%d", i + 1);
        textGLWriteString(num, (xpos + self.arraySegmentSize / 2 + self.screenX) * self.screenSize, (ypos + 17 + self.screenY) * self.screenSize, self.screenSize * self.arraySegmentSize * 0.3, 50);
        xpos += self.arraySegmentSize;
    }
    turtleGoto(((self.toSort -> length / -2.0) * self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize);
    turtlePenDown();
    turtleGoto(((self.toSort -> length / 2.0) * self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize);
    turtleGoto(((self.toSort -> length / 2.0) * self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize);
    turtleGoto(((self.toSort -> length / -2.0) * self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize);
    turtlePenUp();
    *selfp = self;
}

void renderBar(Heapsort *selfp) {
    Heapsort self = *selfp;
    double xpos = (self.toSort -> length / -2.0) * self.arraySegmentSize + self.arraySegmentSize * 0.5;
    double ypos = 170;
    turtlePenSize(10 * self.screenSize);
    for (int i = 0; i < self.toSort -> length; i++) {
        turtlePenColor(0, 0, 0);
        turtleGoto((xpos + self.screenX) * self.screenSize, (ypos + self.screenY) * self.screenSize);
        turtlePenDown();
        turtleGoto((xpos + self.screenX) * self.screenSize, (ypos + self.toSort -> data[i].i * self.barScale + self.screenY) * self.screenSize);
        turtlePenUp();
        xpos += self.arraySegmentSize;
    }
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
    if (turtleKeyPressed(GLFW_KEY_SPACE)) { // space to advance animation
        if (self.keys[1] == 0) {
            self.keys[1] = 1;
            
            if (self.phase) {
                heapify(&self);
            } else {
                createunsorted(&self);
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
    if (turtleKeyPressed(GLFW_KEY_R)) { // R to reset
        if (self.keys[3] == 0) {
            self.keys[3] = 1;
            int len = self.toSort -> length;
            list_free(self.toSort);
            init(selfp, len);
            selfp -> screenX = self.screenX;
            selfp -> screenY = self.screenY;
            selfp -> screenSize = self.screenSize;
            selfp -> keys[3] = 1;
            return;
        }
    } else {
        self.keys[3] = 0;
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

    turtleBgColor(60, 60, 60);
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
        renderHeap(&obj);
        renderBar(&obj);
        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}