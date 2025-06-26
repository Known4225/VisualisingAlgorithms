#include "../include/textGL.h"
#include <time.h>

/* general design plan:
Quicksort visualisation

The bars color and animation are inspired by MathMathMath's scratch project:
https://scratch.mit.edu/projects/149793697/
Which is itself inspired by the original sound of sorting
https://www.youtube.com/watch?v=kPRA0W1kECg
*/

typedef struct {
    char keys[5];
    char phase;
    int operations;
    list_t *toSort;
    list_t *feStack;
    int pivot;
    int loaded;
    int highlight;
    int highlightCyan;
    int pivotSplitter;
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
    int effectiveFront;
    int effectiveEnd;
    double angleChange;
    double barScale;
    list_t *barPosition;
    list_t *barGoto;
    list_t *barLength;
    list_t *barValue;
    list_t *barValueSet;
    list_t *barRecord;
    double animationSpeed;
    double animationValueSpeed;
} Quicksort;

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

void init(Quicksort *selfp, int length) {
    Quicksort self = *selfp;
    self.toSort = list_init();
    self.feStack = list_init();
    for (int i = 0; i < length; i++) {
        list_append(self.toSort, (unitype) randomInt(0, 5 * length), 'i');
    }
    for (int i = 0; i < 5; i++) {
        self.keys[i] = 0;
    }
    self.pivot = -1;
    self.loaded = 0;
    self.highlight = -1;
    self.highlightCyan = -1;
    self.pivotSplitter = -1;
    self.phase = 0;
    self.arraySegmentSize = 20;
    self.effectiveFront = 0;
    self.effectiveEnd = length - 1;
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

    /* bar things */
    self.barScale = 10 / log(length);
    double xpos = (self.toSort -> length / -2.0) * self.arraySegmentSize + self.arraySegmentSize * 0.5;
    self.barPosition = list_init();
    self.barGoto = list_init();
    self.barLength = list_init();
    self.barRecord = list_init();
    self.barValue = list_init();
    self.barValueSet = list_init();
    for (int i = 0; i < length; i++) {
        list_append(self.barPosition, (unitype) xpos, 'd');
        list_append(self.barGoto, (unitype) xpos, 'd');
        list_append(self.barLength, self.toSort -> data[i], 'i');
        list_append(self.barRecord, (unitype) i, 'i');
        list_append(self.barValue, (unitype) 0.9, 'd');
        list_append(self.barValueSet, (unitype) 0.9, 'd');
        xpos += self.arraySegmentSize;
    }
    self.animationSpeed = 0.15;
    self.animationValueSpeed = 0.05;
    *selfp = self;
}

void swap(Quicksort *selfp, int index1, int index2) {
    Quicksort self = *selfp;

    unitype temp = self.toSort -> data[index1];
    self.toSort -> data[index1] = self.toSort -> data[index2];
    self.toSort -> data[index2] = temp;

    int swap1 = -1;
    int swap2 = -1;
    int i = 0;
    while(swap1 < 0 || swap2 < 0) {
        if (self.barRecord -> data[i].i == index1) {
            swap1 = i;
        }
        if (self.barRecord -> data[i].i == index2) {
            swap2 = i;
        }
        i++;
    }
    temp = self.barGoto -> data[swap1];
    self.barGoto -> data[swap1] = self.barGoto -> data[swap2];
    self.barGoto -> data[swap2] = temp;
    temp = self.barRecord -> data[swap1];
    self.barRecord -> data[swap1] = self.barRecord -> data[swap2];
    self.barRecord -> data[swap2] = temp;
    *selfp = self;
}

void quicksortStep(Quicksort *selfp) { // iterative quicksort
    Quicksort self = *selfp;
    if (self.phase == 3) {
        self.phase = -1;
        for (int i = 0; i < self.toSort -> length; i++) {
            self.barValueSet -> data[i].d = 0.9;
        }
        self.pivot = -1;
        self.highlightCyan = self.toSort -> length;
        self.highlight = -1;
        self.pivotSplitter = -1;
        printf("Operations: %d\n", self.operations);
    } else if (self.phase == 2) {
        if (self.effectiveEnd - self.effectiveFront <= 0) {
            
            int find = 0;
            while (self.barRecord -> data[find].i != self.effectiveFront) {
                find++;
            }
            self.barValueSet -> data[find].d = 0.4;
            
            if (self.feStack -> length < 1 || self.feStack -> data[self.feStack -> length - 1].i < self.feStack -> data[self.feStack -> length - 2].i) {
                self.phase = 3;
            } else {
                // list_print(self.feStack);
                self.effectiveEnd = self.feStack -> data[self.feStack -> length - 1].i;
                list_pop(self.feStack);
                self.effectiveFront = self.feStack -> data[self.feStack -> length - 1].i;
                list_pop(self.feStack);
                self.highlightCyan = self.effectiveFront;

                for (int i = self.effectiveFront; i < self.effectiveEnd + 1; i++) {
                    find = 0;
                    while (self.barRecord -> data[find].i != i) {
                        find++;
                    }
                    self.barValueSet -> data[find].d = 0.9;
                }
                self.pivotSplitter = -1;
                self.pivot = -1;
                self.phase = 0;
            }
        } else {
            for (int i = self.pivotSplitter; i < self.effectiveEnd + 1; i++) {
                int find = 0;
                while (self.barRecord -> data[find].i != i) {
                    find++;
                }
                self.barValueSet -> data[find].d = 0.4;
            }

            if (self.pivotSplitter + 1 < self.toSort -> length && (self.effectiveEnd - (self.pivotSplitter + 1) > 0)) {
                list_append(self.feStack, (unitype) (self.pivotSplitter + 1), 'i');
                list_append(self.feStack, (unitype) self.effectiveEnd, 'i');
            }
            self.effectiveEnd = self.pivotSplitter - 1;
            
            self.pivotSplitter = -1;
            self.pivot = -1;
            if (self.effectiveEnd - self.effectiveFront > 0)
                self.phase = 0;
        }
    } else if (self.phase == 1) {
        swap(&self, self.pivotSplitter, self.pivot);
        self.phase = 2;
    } else if (self.phase == 0) {
        if (self.pivot == -1) {
            self.pivot = self.effectiveFront + (self.effectiveEnd - self.effectiveFront) / 2; // middle pivot
            if (self.pivot == self.effectiveFront) {
                self.pivotSplitter = self.pivot + 1;
                self.highlight = self.pivot + 1;
            }
        } else if (self.pivot > self.effectiveFront) {
            swap(&self, self.effectiveFront, self.pivot);
            self.pivot = self.effectiveFront;
            self.pivotSplitter = self.pivot + 1;
            self.highlight = self.pivot + 1;
        } else {
            if (self.toSort -> data[self.highlight].i < self.toSort -> data[self.pivot].i) {
                swap(&self, self.pivotSplitter, self.highlight);
                self.pivotSplitter += 1;
            }
            self.highlight += 1;
            if (self.highlight > self.effectiveEnd) {
                self.highlight = -1;
                self.pivotSplitter -= 1;
                self.phase += 1;
            }
        }
    }
    *selfp = self;
}

void renderArray(Quicksort *selfp) {
    Quicksort self = *selfp;
    double ypos = -165;
    double xpos = (self.toSort -> length / -2.0) * self.arraySegmentSize;
    turtlePenColor(0, 0, 0);
    turtlePenSize(2 * self.screenSize);
    for (int i = 0; i < self.toSort -> length; i++) { 
        if (self.pivotSplitter == i) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            116, 255, 133, 0);
        }
        if (i < self.highlightCyan) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            230, 230, 230, 0);
        }
        if (self.highlight == i) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (ypos - 10 + self.screenY) * self.screenSize,
            19, 236, 48, 0);
        }
        if (self.pivot == i) {
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
        sprintf(num, "%d", i);
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

void renderBar(Quicksort *selfp) {
    Quicksort self = *selfp;
    double ypos = -120;
    turtlePenSize(self.arraySegmentSize * 0.9 * self.screenSize);
    for (int i = 0; i < self.toSort -> length; i++) {
        double hue = ((double) (self.barLength -> data[i].i * 360) / (self.toSort -> length * 5)); // complicated math to convert HSV to RGB
        double saturation = 0.5;
        self.barValue -> data[i].d = self.barValue -> data[i].d + (self.barValueSet -> data[i].d - self.barValue -> data[i].d) * self.animationValueSpeed;
        if (fabs(self.barValueSet -> data[i].d - self.barValue -> data[i].d) < 0.1) {
            self.barValue -> data[i] = self.barValueSet -> data[i];
        }
        double value = self.barValue -> data[i].d;
        double C = value * saturation;
        double X = C * (1 - fabs(fmod((hue / 60), 2) - 1));
        double m = value - C;
        double R;
        double G;
        double B;
        if (hue < 60) {
            R = C;
            G = X;
            B = 0;
        } else if (hue < 120) {
            R = X;
            G = C;
            B = 0;
        } else if (hue < 180) {
            R = 0;
            G = C;
            B = X;
        } else if (hue < 240) {
            R = 0;
            G = X;
            B = C;
        } else if (hue < 300) {
            R = X;
            G = 0;
            B = C;
        } else {
            R = C;
            G = 0;
            B = X;
        }
        turtlePenColor((R + m) * 255, (G + m) * 255, (B + m) * 255);
        self.barPosition -> data[i].d = self.barPosition -> data[i].d + (self.barGoto -> data[i].d - self.barPosition -> data[i].d) * self.animationSpeed;
        turtleGoto((self.barPosition -> data[i].d + self.screenX) * self.screenSize, (ypos + self.screenY) * self.screenSize);
        turtlePenDown();
        turtleGoto((self.barPosition -> data[i].d + self.screenX) * self.screenSize, (ypos + self.barLength -> data[i].i * self.barScale + self.screenY) * self.screenSize);
        turtlePenUp();
        if (fabs(self.barGoto -> data[i].d - self.barPosition -> data[i].d) < 0.1) {
            self.barPosition -> data[i] = self.barGoto -> data[i];
        }
    }
    *selfp = self;
}

void mouseTick(Quicksort *selfp) {
    Quicksort self = *selfp;
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

void scrollTick(Quicksort *selfp) {
    Quicksort self = *selfp;
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

void hotkeyTick(Quicksort *selfp) {
    Quicksort self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE) && self.phase > -1) { // space to advance animation
        if (self.keys[1] == 0) {
            self.operations += 1;
            self.keys[1] = 1;
            if (self.toSort -> length > 1) {
                quicksortStep(&self);
            } else {
                printf("already sorted\n");
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
            list_free(self.feStack);
            list_free(self.barPosition);
            list_free(self.barGoto);
            list_free(self.barLength);
            list_free(self.barRecord);
            list_free(self.barValue);
            list_free(self.barValueSet);
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
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    const GLFWvidmode *monitorSize = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int32_t windowHeight = monitorSize -> height * 0.85;
    GLFWwindow *window = glfwCreateWindow(windowHeight * 16 / 9, windowHeight, "Quicksort", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, windowHeight * 16 / 9, windowHeight, windowHeight * 16 / 9, windowHeight);

    /* initialize turtle */
    turtleInit(window, -320, -180, 320, 180);
    /* initialise textGL */
    textGLInit(window, "../include/fontBez.tgl");

    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;

    turtleBgColor(60, 60, 60);
    Quicksort obj; // principle object
    srand(time(NULL)); // randomiser init seed
    if (argc == 1) {
        init(&obj, 16);
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
        renderBar(&obj);
        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}