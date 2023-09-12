#include "include/textGL.h"
#include <time.h>

/* general design plan:
Mergesort visualisation

Yes, I know it's broken
No, I do not know how to fix it
I want to be done with this one, it's been three days

This one is not as cool as quicksort and heapsort, mainly because it's kind of hard to make the bars animation work with something like this
because of how memory is copied and indexes don't really "swap around" like they do in other sorts.

I do want to eventually put a little thing on the side that lets you step through the merge algorithm, but that will have to wait for another day
this project is a nighmare to work on
I don't like recursion, and should not have used it

*/

typedef struct {
    char keys[5];
    char phase;
    int operations;
    list_t *toSort;
    list_t *sorted;
    list_t *middleCoords;
    list_t *bottomCoords;
    int depth;
    int showDepth;
    double minY;
    int highlight;
    int highlightCyan;
    int contributeGreen;
    int contributeCyan;
    int positionGreen;
    int positionCyan;
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
    int mergeState;
    int mergeStateEnd;
    double barScale;
    list_t *barPosition;
    list_t *barGoto;
    list_t *barLength;
    list_t *barValue;
    list_t *barValueSet;
    list_t *barRecord;
    double animationSpeed;
    double animationValueSpeed;
} Mergesort;

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

int *intMergesortEmb(Mergesort *selfp, int *list, int length, int depth, int start) {
    if (length == 1) { // if the length of the list is 1, it is sorted by default
        return list;
    }
    int halfLen = length / 2;
    int *list1;
    int *list2;
    list1 = intMergesortEmb(selfp, list, halfLen, depth + 1, start); // assume the left half is sorted after this call
    list2 = intMergesortEmb(selfp, list + halfLen, length - halfLen, depth + 1, start + halfLen); // assume right half is sorted after this call
    int j = start;
    for (int i = 0; i < halfLen; i++) {
        selfp -> sorted -> data[depth].r -> data[j].i = list1[i];
        j++;
    }
    for (int i = 0; i < halfLen; i++) {
        selfp -> sorted -> data[depth].r -> data[j].i = list2[i];
        j++;
    }
    

    /* Merge */

    int *listEnd = malloc(sizeof(int) * length);
    int list1p = 0;
    int list2p = 0;
    for (int i = 0; i < length; i++) {
        if (list1p >= halfLen) {
            listEnd[i] = list2[list2p];
            list2p++;
        } else if (list2p >= length - halfLen) {
            listEnd[i] = list1[list1p];
            list1p++;
        } else {
            if (list1[list1p] > list2[list2p]) {
                listEnd[i] = list2[list2p];
                list2p++;
            } else {
                listEnd[i] = list1[list1p];
                list1p++;
            }
        }
    }
    /* i don't know why this works */
    if (length > 3) {
        free(list1);
    }
    if (length > 2) {
        free(list2);
    }
    return listEnd;
}

void intMergesort(Mergesort *selfp, list_t *list) {
    if (list -> length > 1) {
        int *oldList = malloc(sizeof(int) * list -> length);
        for (int i = 0; i < list -> length; i++) {
            oldList[i] = list -> data[i].i;
        }
        int *newList = intMergesortEmb(selfp, oldList, list -> length, 1, 0);
        free(oldList);
        for (int i = 0; i < list -> length; i++) {
            list -> data[i].i = newList[i];
        }
        free(newList);
    }
}

void init(Mergesort *selfp, int length) {
    Mergesort self = *selfp;
    self.toSort = list_init();
    self.bottomCoords = list_init();
    self.middleCoords = list_init();
    for (int i = 0; i < length; i++) {
        list_append(self.toSort, (unitype) randomInt(0, 5 * length), 'i');
    }
    for (int i = 0; i < 5; i++) {
        self.keys[i] = 0;
    }
    self.depth = (int) ceil(log(length) / log(2));
    self.showDepth = self.depth + 1;
    self.mergeState = pow(2, self.depth) * 5 - 2 * 5; // stride
    self.mergeStateEnd = 0;
    self.sorted = list_init();
    for (int i = 0; i < self.depth + 1; i++) {
        list_append(self.sorted, (unitype) list_init(), 'r');
        for (int j = 0; j < length; j++) {
            list_append(self.sorted -> data[i].r, self.toSort -> data[j], 'i');
        }
    }
    list_t *dummy = list_init();
    list_copy(self.toSort, dummy);
    intMergesort(&self, self.sorted -> data[0].r);
    free(dummy);



    self.minY = 0;
    self.highlight = -1;
    self.highlightCyan = -1;
    self.contributeGreen = 0;
    self.contributeCyan = 0;
    self.positionGreen = 0;
    self.positionCyan = 0;
    self.phase = 0;
    self.arraySegmentSize = 20;
    self.effectiveFront = 0;
    self.effectiveEnd = length - 1;
    

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

void swap(Mergesort *selfp, int index1, int index2) {
    Mergesort self = *selfp;

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

void mergesortStep(Mergesort *selfp) { // iterative Mergesort
    Mergesort self = *selfp;
    if (self.phase == 3) {

    } else if (self.phase == 2) {
        /* finished */
    } else if (self.phase == 1) {
        if (self.mergeState > -1) {
            if (self.mergeStateEnd == 0) {
                self.mergeState -= 5; // stride
                self.positionGreen = 0;
                self.positionCyan = 0;
                self.mergeStateEnd = self.middleCoords -> data[self.mergeState + 5 + 3].i - self.middleCoords -> data[self.mergeState + 5 + 2].i; // stride
            } else {
                if (self.contributeGreen < self.contributeCyan) {
                    self.positionGreen += 1;
                } else {
                    self.positionCyan += 1;
                }
                self.mergeStateEnd -= 1;
            }
        } else {
            if (self.mergeStateEnd > 0) {
                if (self.contributeGreen < self.contributeCyan) {
                    self.positionGreen += 1;
                } else {
                    self.positionCyan += 1;
                }
                self.mergeStateEnd -= 1;
            } else {
                printf("operations: %d\n", self.operations);
                self.phase = 2;
            }
        }  
    } else if (self.phase == 0) {
        self.showDepth -= 1;
        if (self.showDepth == 1) {
            self.phase = 1;
        }
    }
    *selfp = self;
}

void drawArrow(Mergesort *selfp, double x1, double y1, double x2, double y2) {
    Mergesort self = *selfp;
    turtlePenSize(2 * self.screenSize);
    turtleGoto((x1 + self.screenX) * self.screenSize, (y1 + self.screenY) * self.screenSize);
    turtlePenDown();
    turtleGoto((x2 + self.screenX) * self.screenSize, (y2 + self.screenY) * self.screenSize);
    double angle = atan((x1 - x2) / (y1 - y2)) * 57.2958;
    if (y2 > y1) {
        angle += 180;
    }
    turtleGoto((x2 + 5 * sin((angle - 45) / 57.2958) + self.screenX) * self.screenSize, (y2 + 5 * cos((angle - 45) / 57.2958) + self.screenY) * self.screenSize);
    turtlePenUp();
    turtleGoto((x2 + self.screenX) * self.screenSize, (y2 + self.screenY) * self.screenSize);
    turtlePenDown();
    turtleGoto((x2 + 5 * sin((angle + 45) / 57.2958) + self.screenX) * self.screenSize, (y2 + 5 * cos((angle + 45) / 57.2958) + self.screenY) * self.screenSize);
    turtlePenUp();
}

void renderArray(Mergesort *selfp, double x, double y, int start, int end, char array, int specialGreen, int specialCyan) {
    Mergesort self = *selfp;
    turtlePenSize(2 * self.screenSize);
    double xpos = x + ((end - start + 1) / -2.0) * self.arraySegmentSize;
    for (int i = start; i < end + 1; i++) {
        if (specialGreen == i) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (y + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (y + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (y - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (y - 10 + self.screenY) * self.screenSize,
            19, 236, 48, 0);
        }
        if (specialCyan == i) {
            turtleQuad((xpos + self.screenX) * self.screenSize, (y + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (y + 10 + self.screenY) * self.screenSize,
            (xpos + self.arraySegmentSize + self.screenX) * self.screenSize, (y - 10 + self.screenY) * self.screenSize,
            (xpos + self.screenX) * self.screenSize, (y - 10 + self.screenY) * self.screenSize,
            43, 188, 255, 0);
        }
        turtleGoto((xpos + self.screenX) * self.screenSize, (y + 10 + self.screenY) * self.screenSize);
        turtlePenDown();
        turtleGoto((xpos + self.screenX) * self.screenSize, (y - 10 + self.screenY) * self.screenSize);
        turtlePenUp();
        char num[12];
        if (array == 1)
            sprintf(num, "%d", self.toSort -> data[i].i);
        else
            sprintf(num, "%d", self.sorted -> data[array - 2].r -> data[i].i);
        textGLWriteString(num, (xpos + self.arraySegmentSize / 2 + self.screenX) * self.screenSize, (y + self.screenY) * self.screenSize, self.screenSize * self.arraySegmentSize * 0.5, 50);
        sprintf(num, "%d", i);
        textGLWriteString(num, (xpos + self.arraySegmentSize / 2 + self.screenX) * self.screenSize, (y + 17 + self.screenY) * self.screenSize, self.screenSize * self.arraySegmentSize * 0.3, 50);
        xpos += self.arraySegmentSize;
    }
    turtleGoto((((end - start + 1) / -2.0) * self.arraySegmentSize + self.screenX + x) * self.screenSize, (y + 10 + self.screenY) * self.screenSize);
    turtlePenDown();
    turtleGoto((((end - start + 1) / 2.0) * self.arraySegmentSize + self.screenX + x) * self.screenSize, (y + 10 + self.screenY) * self.screenSize);
    turtleGoto((((end - start + 1) / 2.0) * self.arraySegmentSize + self.screenX + x) * self.screenSize, (y - 10 + self.screenY) * self.screenSize);
    turtleGoto((((end - start + 1) / -2.0) * self.arraySegmentSize + self.screenX + x) * self.screenSize, (y - 10 + self.screenY) * self.screenSize);
    turtlePenUp();
}

void renderMergeArray(Mergesort *selfp, double x, double y, int start, int end, int depth) { // recursive graphics
    Mergesort self = *selfp;
    turtlePenColor(0, 0, 0);
    if (depth > self.showDepth - 1) {
        list_append(self.middleCoords, (unitype) x, 'd');
        list_append(self.middleCoords, (unitype) y, 'd');
        list_append(self.middleCoords, (unitype) start, 'i');
        list_append(self.middleCoords, (unitype) end, 'i');
        list_append(self.middleCoords, (unitype) depth, 'i');
        renderArray(selfp, x, y, start, end, 1, -1, -1);
        double length = pow(2, ceil(log(end - start + 1) / log(2)));
        int length1 = ((end - start + 1) / 2);
        int length2 = end - start - length1 + 1;
        int newLength = pow(2, ceil(log(length2) / log(2)));
        if (end - start > 0) {
            drawArrow(selfp, x - (length / 4.0) * self.arraySegmentSize, y - 20, x - (length / 4.0 + 0.04 * length) * self.arraySegmentSize, y - 40);
            drawArrow(selfp, x + (length / 4.0) * self.arraySegmentSize, y - 20, x + (length / 4.0 + 0.04 * length) * self.arraySegmentSize, y - 40);
            renderMergeArray(selfp, x + (newLength / 2.0 + 0.1 * newLength) * self.arraySegmentSize, y - 70, start + length1, end, depth - 1);
            renderMergeArray(selfp, x - (newLength / 2.0 + 0.1 * newLength) * self.arraySegmentSize, y - 70, start, start + length1 - 1, depth - 1);
        } else {
            drawArrow(selfp, x, y - 20, x, y - 40);
            renderMergeArray(selfp, x, y - 70, start, end, depth - 1);
        }
    } else {
        int powTwo = floor(pow(2, (self.middleCoords -> data[self.mergeState + 5 + 4].i)));
        if ((int) self.middleCoords -> length - 5 > self.mergeState && powTwo == 2) {
            if (start == self.middleCoords -> data[self.mergeState + 5 + 2].i) {
                if (self.positionGreen > 0) {
                    selfp -> contributeGreen = self.toSort -> length * 10;
                    renderArray(selfp, x, y, start, end, 1, -1, -1);
                } else {
                    selfp -> contributeGreen = self.toSort -> data[start].i;
                    renderArray(selfp, x, y, start, end, 1, start, -1);
                }
            } else if (start == self.middleCoords -> data[self.mergeState + 5 + 2].i + 1) {
                if (self.positionCyan > 0) {
                    selfp -> contributeCyan = self.toSort -> length * 10;
                    renderArray(selfp, x, y, start, end, 1, -1, -1);
                } else {
                    selfp -> contributeCyan = self.toSort -> data[start].i;
                    renderArray(selfp, x, y, start, end, 1, -1, start);
                }
            } else {
                renderArray(selfp, x, y, start, end, 1, -1, -1);
            }
        } else {
            renderArray(selfp, x, y, start, end, 1, -1, -1);
        }
        list_append(self.bottomCoords, (unitype) x, 'd');
        list_append(self.bottomCoords, (unitype) y, 'd');
    }
}

void renderMerging(Mergesort *selfp) {
    Mergesort self = *selfp;
    turtlePenColor(120, 120, 120);
    for (int i = self.middleCoords -> length - 5; i > self.mergeState; i -= 5) { // stride
        double x = self.middleCoords -> data[i].d;
        double y = self.minY - (self.middleCoords -> data[i + 1].d - self.minY);
        int start = self.middleCoords -> data[i + 2].i;
        int end = self.middleCoords -> data[i + 3].i;
        int arraySelect = 2 + (self.depth - self.middleCoords -> data[i + 4].i);
        double length = pow(2, ceil(log(end - start + 1) / log(2)));
        if (i == self.mergeState + 5) { // stride
            end -= self.mergeStateEnd;
            if (self.phase > 1) {
                renderArray(selfp, x, y, start, end, arraySelect, -1, -1);
            } else {
                if (self.contributeGreen < self.contributeCyan) {
                    renderArray(selfp, x, y, start, end, arraySelect, end, -1);
                } else {
                    renderArray(selfp, x, y, start, end, arraySelect, -1, end);
                }
            }
        }
        int parent1 = self.mergeState + 10; // stride
        int powTwo = floor(pow(2, (self.middleCoords -> data[self.mergeState + 5 + 4].i)));
        int parent2 = self.mergeState + 5 + 5 * powTwo / 2; // stride
        if (i == parent1 && self.phase < 2 && powTwo != 2) {
            if (start + self.positionCyan > end) {
                self.contributeCyan = self.toSort -> length * 10;
            } else {
                self.contributeCyan = self.sorted -> data[arraySelect - 2].r -> data[start + self.positionCyan].i;
            }
            renderArray(selfp, x, y, start, end, arraySelect, -1, start + self.positionCyan);
        } else if (i == parent2 && self.phase < 2 && powTwo != 2) {
            if (start + self.positionGreen > end) {
                self.contributeGreen = self.toSort -> length * 10;
            } else {
                self.contributeGreen = self.sorted -> data[arraySelect - 2].r -> data[start + self.positionGreen].i;
            }
            renderArray(selfp, x, y, start, end, arraySelect, start + self.positionGreen, -1);
        } else {
            renderArray(selfp, x, y, start, end, arraySelect, -1, -1);
        }
        if (end - start > 0) {
            drawArrow(selfp, x - (length / 4.0 + 0.04 * length) * self.arraySegmentSize, y + 50, x - (length / 4.0) * self.arraySegmentSize, y + 30);
            drawArrow(selfp, x + (length / 4.0 + 0.04 * length) * self.arraySegmentSize, y + 50, x + (length / 4.0) * self.arraySegmentSize, y + 30);
        } else {
            drawArrow(selfp, x, y + 50, x, y + 30);
        }
    }
    *selfp = self;
}

void renderBar(Mergesort *selfp) {
    Mergesort self = *selfp;
    double ypos = 190;
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

void mouseTick(Mergesort *selfp) {
    Mergesort self = *selfp;
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

void scrollTick(Mergesort *selfp) {
    Mergesort self = *selfp;
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

void hotkeyTick(Mergesort *selfp) {
    Mergesort self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE) && self.phase > -1) { // space to advance animation
        if (self.keys[1] == 0) {
            self.operations += 1;
            self.keys[1] = 1;
            if (self.toSort -> length > 1) {
                mergesortStep(&self);
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
            list_free(self.sorted);
            list_free(self.bottomCoords);
            list_free(self.middleCoords);
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
    GLFWwindow* window;
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Mergesort", NULL, NULL);
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
    Mergesort obj; // principle object
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
        list_clear(obj.bottomCoords);
        list_clear(obj.middleCoords);
        renderMergeArray(&obj, 0, 150, 0, obj.toSort -> length - 1, obj.depth);
        if (obj.phase > 0) {
            if (obj.minY == 0) {
                for (int i = 1; i < obj.bottomCoords -> length; i += 2) {
                    if (obj.bottomCoords -> data[i].d < obj.minY) {
                        obj.minY = obj.bottomCoords -> data[i].d;
                    }
                }
            }
            renderMerging(&obj);
        }
        // renderBar(&obj);
        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}