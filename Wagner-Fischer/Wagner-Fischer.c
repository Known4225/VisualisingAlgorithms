#include "../include/textGL.h"
#include <time.h>

typedef struct {
    char keys[5];
    char showPaths;
    char completed;
    char *string1; // string to be analysed
    char *string2; // string to be analysed
    int dimensionX; // length of string1
    int dimensionY; // length of string2
    int max;
    double boxSize; // for rendering
    double screenX;
    double screenY;
    double globalsize;
    double scrollSpeed;
    double focalX;
    double focalY;
    double focalCSX;
    double focalCSY;
    list_t *matrix; // data
} WagnerFischer; // i keep spelling fisher without the c

void init(WagnerFischer *selfp) {
    WagnerFischer self = *selfp;
    self.boxSize = 30;
    self.screenX = 0;
    self.screenY = 0;
    self.globalsize = 1;
    self.scrollSpeed = 1.15;
    self.completed = 0;
    self.showPaths = 0;
    self.max = 0;
    for (int i = 0; i < 5; i++) {
        self.keys[i] = 0;
    }
    *selfp = self;
}

void drawGrid(WagnerFischer *selfp) {
    WagnerFischer self = *selfp;
    turtlePenColor(200, 200, 200);
    char singleChar[2] = {'\0', '\0'};
    double xpos = self.dimensionX * -0.5 * self.boxSize + self.boxSize * 0.5;
    double ypos = 120 - self.boxSize;
    for (int i = 0; i < self.dimensionX; i++) {
        xpos += self.boxSize;
        singleChar[0] = self.string1[i];
        textGLWriteString(singleChar, (xpos + self.screenX) * self.globalsize, (ypos + self.boxSize + self.screenY) * self.globalsize, self.boxSize * 0.8 * self.globalsize, 50);
    }
    xpos = self.dimensionX * -0.5 * self.boxSize - self.boxSize * 0.5;
    for (int i = 0; i < self.dimensionY; i++) {
        ypos -= self.boxSize;
        singleChar[0] = self.string2[i];
        textGLWriteString(singleChar, (xpos + self.screenX) * self.globalsize, (ypos + self.screenY) * self.globalsize, self.boxSize * 0.8 * self.globalsize, 50);
    }
    turtlePenSize(self.globalsize);
    turtlePenColor(20, 20, 20);
    xpos = self.dimensionX * -0.5 * self.boxSize - self.boxSize;
    ypos = 120 + self.boxSize * 0.55;
    double endX = self.dimensionX * 0.5 * self.boxSize;
    for (int i = 0; i < self.dimensionY + 2; i++) {
        turtleGoto((xpos + self.screenX) * self.globalsize, (ypos + self.screenY) * self.globalsize);
        turtlePenDown();
        turtleGoto((endX + self.screenX) * self.globalsize, (ypos + self.screenY) * self.globalsize);
        turtlePenUp();
        ypos -= self.boxSize;
    }
    double endY = ypos + self.boxSize;
    xpos = self.dimensionX * -0.5 * self.boxSize - self.boxSize;
    ypos = 120 + self.boxSize * 0.55;
    for (int i = 0; i < self.dimensionX + 2; i++) {
        turtleGoto((xpos + self.screenX) * self.globalsize, (ypos + self.screenY) * self.globalsize);
        turtlePenDown();
        turtleGoto((xpos + self.screenX) * self.globalsize, (endY + self.screenY) * self.globalsize);
        turtlePenUp();
        xpos += self.boxSize;
    }
    *selfp = self;

    /* draw numbers in boxes */
    turtlePenColor(120, 120, 120);
    ypos = 120;
    char number[12]; // max string length for integer
    for (int i = 0; i < self.dimensionY; i++) {
        xpos = self.dimensionX * -0.5 * self.boxSize - self.boxSize * 0.5;
        ypos -= self.boxSize;
        for (int j = 0; j < self.dimensionX; j++) {
            xpos += self.boxSize;
            if (self.matrix -> data[i].r -> data[j].i != -1) {
                sprintf(number, "%d", self.matrix -> data[i].r -> data[j].i);
                textGLWriteString(number, (xpos + self.screenX) * self.globalsize, (ypos + self.screenY) * self.globalsize, self.boxSize * 0.6 * self.globalsize, 50);
            }
        }
    }
}

void fillBox(WagnerFischer *selfp, int x, int y, double r, double g, double b, double a) { // color a specific box
    WagnerFischer self = *selfp;
    double xpos = self.dimensionX * -0.5 * self.boxSize;
    double ypos = 120 - self.boxSize * 0.45;
    turtleQuad((xpos + self.boxSize * x + self.screenX) * self.globalsize, (ypos - self.boxSize * y + self.screenY) * self.globalsize, (xpos + self.boxSize * x + self.screenX) * self.globalsize, (ypos - self.boxSize * (y + 1) + self.screenY) * self.globalsize, (xpos + self.boxSize * (x + 1) + self.screenX) * self.globalsize, (ypos - self.boxSize * (y + 1) + self.screenY) * self.globalsize, (xpos + self.boxSize * (x + 1) + self.screenX) * self.globalsize, (ypos - self.boxSize * y + self.screenY) * self.globalsize, r, g, b, a);
    // no state change
}

void annotateMatrix(WagnerFischer *selfp, int x, int y) { // colors the matrix, prepares for fill (visually)
    WagnerFischer self = *selfp;
    if (self.string1[x - 1] == self.string2[y - 1]) {
        fillBox(&self, x, -1, 0, 255, 0, 0);
        fillBox(&self, -1, y, 0, 255, 0, 0);
        fillBox(&self, x - 1, y - 1, 255, 0, 0, 0);
    } else {
        fillBox(&self, x, -1, 255, 255, 0, 0);
        fillBox(&self, -1, y, 255, 255, 0, 0);
        int min = 2147483647;
        int sort[3];
        sort[0] = self.matrix -> data[y - 1].r -> data[x].i;
        sort[1] = self.matrix -> data[y].r -> data[x - 1].i;
        sort[2] = self.matrix -> data[y - 1].r -> data[x - 1].i;
        for (int i = 0; i < 3; i++) {
            if (sort[i] < min) {
                min = sort[i];
            }
        }
        if (sort[0] == min) {
            fillBox(&self, x, y - 1, 255, 0, 0, 0);
        } else {
            fillBox(&self, x, y - 1, 100, 0, 0, 0);
        }
        if (sort[1] == min) {
            fillBox(&self, x - 1, y, 255, 0, 0, 0);
        } else {
            fillBox(&self, x - 1, y, 100, 0, 0, 0);
        }
        if (sort[2] == min) {
            fillBox(&self, x - 1, y - 1, 255, 0, 0, 0);
        } else {
            fillBox(&self, x - 1, y - 1, 100, 0, 0, 0);
        }
    }
    fillBox(&self, x, y, 0, 0, 0, 0);
    // no state change
}

void showPaths(WagnerFischer *selfp) {
    WagnerFischer self = *selfp;
    double factor = 200.0 / self.max;
    for (int i = 0; i < self.dimensionX; i++) {
        for (int j = 0; j < self.dimensionY; j++) {
            fillBox(&self, i, j, 0, 0, 255 - self.matrix -> data[j].r -> data[i].i * factor, 0);
        }
    }
    *selfp = self;
}

void fillMatrix(WagnerFischer *selfp, int x, int y) { // fills a box of the matrix
    WagnerFischer self = *selfp;
    //printf("%c %c\n", self.string1[x - 1], self.string2[y - 1]);
    if (self.string1[x - 1] == self.string2[y - 1]) {
        /* take diagonal value */
        self.matrix -> data[y].r -> data[x] = self.matrix -> data[y - 1].r -> data[x - 1];
    } else {
        /* take minimum of three surrounding boxes + 1 */
        int up = self.matrix -> data[y - 1].r -> data[x].i;
        int left = self.matrix -> data[y].r -> data[x - 1].i;
        int dia = self.matrix -> data[y - 1].r -> data[x - 1].i;
        if (up > left) {
            up = left;
        }
        if (up > dia) {
            up = dia;
        }
        // up is now the minimum
        self.matrix -> data[y].r -> data[x] = (unitype) (up + 1);
    }
    if (self.matrix -> data[y].r -> data[x].i > self.max) {
        self.max = self.matrix -> data[y].r -> data[x].i;
    }
    *selfp = self;
}

void mouseTick(WagnerFischer *selfp, double mouseX, double mouseY) {
    WagnerFischer self = *selfp;
    if (turtleMouseDown()) {
        if (self.keys[0] == 0) {
            self.keys[0] = 1;
            self.focalX = mouseX;
            self.focalY = mouseY;
            self.focalCSX = self.screenX;
            self.focalCSY = self.screenY;
        } else {
            self.screenX = (mouseX - self.focalX) / self.globalsize + self.focalCSX;
            self.screenY = (mouseY - self.focalY) / self.globalsize + self.focalCSY;
        }
    } else {
        self.keys[0] = 0;
    }
    *selfp = self;
}

void scrollTick(WagnerFischer *selfp, double mouseWheel) {
    WagnerFischer self = *selfp;
    if (mouseWheel > 0) {
        self.screenX -= (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.globalsize;
        self.screenY -= (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.globalsize;
        self.globalsize *= self.scrollSpeed;
    }
    if (mouseWheel < 0) {
        self.globalsize /= self.scrollSpeed;
        self.screenX += (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.globalsize;
        self.screenY += (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.globalsize;
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
    window = glfwCreateWindow(1280, 720, "Wagner-Fischer", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, 1280, 720);

    /* initialize turtle */
    turtleInit(window, -320, -180, 320, 180);
    /* initialise textGL */
    textGLInit(window, "../include/fontBez.tgl");

    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;

    turtleBgColor(50, 50, 50);
    WagnerFischer obj; // principle object
    init(&obj);

    /* parse arguments */
    if (argc > 2) {
        obj.dimensionX = strlen(argv[1]) + 1;
        obj.string1 = malloc(sizeof(char) * obj.dimensionX);
        memcpy(obj.string1, argv[1], obj.dimensionX);
        obj.dimensionY = strlen(argv[2]) + 1;
        obj.string2 = malloc(sizeof(char) * obj.dimensionY);
        memcpy(obj.string2, argv[2], obj.dimensionY);
    } else {
        turtlePenColor(200, 200, 200);
        textGLWriteString("Please use console to input strings", 0, 0, 20, 50);
        turtleUpdate();
        obj.string1 = malloc(sizeof(char) * 1024);
        obj.string2 = malloc(sizeof(char) * 1024);
        printf("string1: ");
        fgets(obj.string1, 1024, stdin);
        obj.dimensionX = strlen(obj.string1);
        printf("string2: ");
        fgets(obj.string2, 1024, stdin);
        obj.dimensionY = strlen(obj.string2);
    }

    /* initialise matrix */
    obj.matrix = list_init();
    for (int i = 0; i < obj.dimensionY; i++) {
        list_append(obj.matrix, (unitype) list_init(), 'r'); // create dimensionY lists inside the list
        for (int j = 0; j < obj.dimensionX; j++) {
            if (i == 0) {
                list_append(obj.matrix -> data[i].r, (unitype) j, 'i');
            } else {
                if (j == 0) {
                    list_append(obj.matrix -> data[i].r, (unitype) i, 'i');
                } else {
                    list_append(obj.matrix -> data[i].r, (unitype) -1, 'i'); // add -1s to the values of the inner lists (to signifiy not set),
                }
            }
        }
    }
    

    int pointX = 1; // keeps track of which box we are editing
    int pointY = 1;
    while (turtle.close == 0) { // main loop
        start = clock();
        turtleClear();
        turtleGetMouseCoords(); // get the mouse coordinates (turtle.mouseX, turtle.mouseY)
        double mouseWheel = turtleMouseWheel(); // behavior is a bit different for the scroll wheel
        mouseTick(&obj, turtle.mouseX, turtle.mouseY);
        scrollTick(&obj, mouseWheel);
        
        //textGLWriteString(obj.string1, 0, 160, 20, 50);
        //textGLWriteString(obj.string2, 0, 120, 20, 50);
        if (turtleKeyPressed(GLFW_KEY_SPACE)) {
            if (obj.keys[1] == 0 || obj.keys[1] == -1) {
                if (obj.completed) {
                    obj.showPaths = !obj.showPaths;
                } else {
                    fillMatrix(&obj, pointX, pointY);
                    pointX += 1;
                    if (pointX >= obj.dimensionX) {
                        pointX = 1;
                        pointY += 1;
                        if (pointY >= obj.dimensionY) {
                            obj.completed = 1;
                        }
                    }
                }
            }
            if (obj.keys[1] > -1) {
                obj.keys[1] += 1;
                if (obj.keys[1] > 20) {
                    obj.keys[1] = -1;
                }
            }
        } else {
            obj.keys[1] = 0;
        }
        if (!obj.completed) {
            annotateMatrix(&obj, pointX, pointY);
        }
        if (obj.showPaths) {
            showPaths(&obj);
        }
        drawGrid(&obj);

        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}