/* openGL implementation of turtle and the turtletools module
features:
adjustable pen (size and colour)
resizable window support
keyboard and mouse presses
*/

#ifndef TURTLESET
#define TURTLESET 1 // include guard
#include <math.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include "glad.h"
#include "glfw3.h"
#include "list.h"

extern void glColor4d(double r, double g, double b, double a); // genius tactic to stop compiler warnings
extern void glBegin(int type);
extern void glVertex2d(double x, double y);
extern void glEnd();

typedef struct {
    GLFWwindow* window; // the window
    char close;
    list_t *keyPressed; // global keyPressed and mousePressed list
    int screenbounds[2]; // list of screen bounds (pixels)
    int lastscreenbounds[2]; // list of screen bounds last frame
    int initscreenbounds[2]; // screenbounds at initialisation
    int bounds[4]; // list of coordinate bounds (minX, minY, maxX, maxY)
    double mouseX; // mouseX and mouseY variables
    double mouseY;
    double scrollY;
    double mouseScaX;
    double mouseScaY;
    double mouseAbsX;
    double mouseAbsY;
    list_t *penPos; // a list of where to draw
    double altSum; // a first line of defense to check if anything has changed frame by frame (unused)
    list_t *penPosOld; // this list syncs with penPos every frame and if nothing changes from one frame to the next, the screen is not redrawn
    double x; // x and y position of the turtle
    double y;
    char pen; // pen status (1 for down, 0 for up)
    char penshape; // 0 for circle, 1 for square, 2 for triangle
    double circleprez; // how precise circles are (specifically, the number of sides of a circle with diameter e)
    double pensize; // turtle pen size
    double penr;
    double peng;
    double penb;
    double pena;
    double currentColor[4]; // for reducing API color calls
} turtleglob; // all globals are conSTRUCTed here

turtleglob turtle;

void turtleSetWorldCoordinates(int minX, int minY, int maxX, int maxY) { // run this to set the bounds of the window in coordinates
    glfwGetWindowSize(turtle.window, &turtle.screenbounds[0], &turtle.screenbounds[1]);
    turtle.initscreenbounds[0] = turtle.screenbounds[0];
    turtle.initscreenbounds[1] = turtle.screenbounds[1];
    turtle.bounds[0] = minX;
    turtle.bounds[1] = minY;
    turtle.bounds[2] = maxX;
    turtle.bounds[3] = maxY;
}
void keySense(GLFWwindow* window, int key, int scancode, int action, int mods) { // detect key presses
    if (action == GLFW_PRESS) {
        list_append(turtle.keyPressed, (unitype) key, 'i');
    }
    if (action == GLFW_RELEASE) {
        list_remove(turtle.keyPressed, (unitype) key, 'i');
    }
}
void mouseSense(GLFWwindow* window, int button, int action, int mods) { // detect mouse clicks
    if (action == GLFW_PRESS) {
        switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT:
            list_append(turtle.keyPressed, (unitype) "m1", 's');
            break;
            case GLFW_MOUSE_BUTTON_RIGHT:
            list_append(turtle.keyPressed, (unitype) "m2", 's');
            break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
            list_append(turtle.keyPressed, (unitype) "m3", 's');
            break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT:
            list_remove(turtle.keyPressed, (unitype) "m1", 's');
            break;
            case GLFW_MOUSE_BUTTON_RIGHT:
            list_remove(turtle.keyPressed, (unitype) "m2", 's');
            break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
            list_remove(turtle.keyPressed, (unitype) "m3", 's');
            break;
        }
    }
}
void scrollSense(GLFWwindow* window, double xoffset, double yoffset) {
    turtle.scrollY = yoffset;
}
double turtleMouseWheel() { // the behavior with the mouse wheel is different since it can't be "on" or "off"
    double temp = turtle.scrollY;
    turtle.scrollY = 0;
    return temp;
}
char turtleKeyPressed(int key) { // top level boolean output call to check if the key with code [key] is currently being held down
    return list_count(turtle.keyPressed, (unitype) key, 'c');
}
char turtleMouseDown() { // top level boolean output call to check if the left click button is currently being held down
    return list_count(turtle.keyPressed, (unitype) "m1", 's');
}
char turtleMouseRight() { // top level boolean output call to check if the right click button is currently being held down
    return list_count(turtle.keyPressed, (unitype) "m2", 's');
}
char turtleMouseMiddle() { // top level boolean output call to check if the middle mouse button is currently being held down
    return list_count(turtle.keyPressed, (unitype) "m3", 's');
}
char turtleMouseMid() { // alternate duplicate of above
    return list_count(turtle.keyPressed, (unitype) "m3", 's');
}
void turtleInit(GLFWwindow* window, int minX, int minY, int maxX, int maxY) { // initializes the turtletools module
    gladLoadGL();
    glfwMakeContextCurrent(window); // various glfw things
    glEnable(GL_ALPHA);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    glClearColor(1.0, 1.0, 1.0, 0.0); // white background by default
    turtle.window = window;
    turtle.close = 0;
    turtle.keyPressed = list_init();
    turtle.lastscreenbounds[0] = 0;
    turtle.lastscreenbounds[1] = 0;
    turtle.penPos = list_init();
    turtle.penPosOld = list_init();
    turtle.x = 0;
    turtle.y = 0;
    turtle.pensize = 1;
    turtle.penshape = 0;
    turtle.circleprez = 9; // default circleprez value
    turtle.pen = 0;
    turtle.penr = 0.0;
    turtle.peng = 0.0;
    turtle.penb = 0.0;
    turtle.pena = 0.0;
    for (int i = 0; i < 3; i++) {
        turtle.currentColor[i] = 0.0;
    }
    turtle.currentColor[3] = 1.0;
    turtleSetWorldCoordinates(minX, minY, maxX, maxY);
    glfwSetKeyCallback(window, keySense); // initiate mouse and keyboard detection
    glfwSetMouseButtonCallback(window, mouseSense);
    glfwSetScrollCallback(window, scrollSense);
}
void turtleGetMouseCoords() { // gets the mouse coordinates
    glfwGetWindowSize(turtle.window, &turtle.screenbounds[0], &turtle.screenbounds[1]); // get screenbounds
    glfwGetCursorPos(turtle.window, &turtle.mouseAbsX, &turtle.mouseAbsY); // get mouse positions (absolute)
    turtle.mouseX = turtle.mouseAbsX;
    turtle.mouseY = turtle.mouseAbsY;
    turtle.mouseScaX = turtle.mouseAbsX;
    turtle.mouseScaY = turtle.mouseAbsY;
    turtle.mouseX -= (turtle.initscreenbounds[0] / 2) - ((turtle.bounds[2] + turtle.bounds[0]) / 2);
    turtle.mouseX *= ((double) (turtle.bounds[2] - turtle.bounds[0]) / (double) turtle.initscreenbounds[0]);
    turtle.mouseY -= (turtle.initscreenbounds[1] / 2) - ((turtle.bounds[3] + turtle.bounds[1]) / 2) + (turtle.screenbounds[1] - turtle.initscreenbounds[1]);
    turtle.mouseY *= ((double) (turtle.bounds[1] - turtle.bounds[3]) / (double) turtle.initscreenbounds[1]);
    turtle.mouseScaX -= (turtle.screenbounds[0] / 2) - ((turtle.bounds[2] + turtle.bounds[0]) / 2);
    turtle.mouseScaX *= ((double) (turtle.bounds[2] - turtle.bounds[0]) / (double) turtle.screenbounds[0]);
    turtle.mouseScaY -= (turtle.screenbounds[1] / 2) - ((turtle.bounds[3] + turtle.bounds[1]) / 2);
    turtle.mouseScaY *= ((double) (turtle.bounds[1] - turtle.bounds[3]) / (double) turtle.screenbounds[1]);
}
void turtleBgColor(double r, double g, double b) { // set the background color
    glClearColor(r / 255, g / 255, b / 255, 0.0);
}
void turtlePenColor(double r, double g, double b) { // set the pen color
    turtle.penr = r / 255;
    turtle.peng = g / 255;
    turtle.penb = b / 255;
    turtle.pena = 0.0;
}
void turtlePenColorAlpha(double r, double g, double b, double a) { // set the pen color (with transparency)
    turtle.penr = r / 255;
    turtle.peng = g / 255;
    turtle.penb = b / 255;
    turtle.pena = a / 255;
}
void turtlePenSize(double size) {
    turtle.pensize = size * 0.5; // ensures pensize matches pixel size (a pen size of 240 will be 240 coordinates long)
}
void turtleClear() { // clears all the pen drawings
    list_free(turtle.penPos);
    turtle.penPos = list_init();
}
void turtlePenDown() {
    if (turtle.pen == 0) {
        turtle.pen = 1;
        char changed = 0;
        int len = turtle.penPos -> length;
        if (len > 0) {
            unitype *ren = turtle.penPos -> data;
            if (ren[len - 9].d != turtle.x) {changed = 1;}
            if (ren[len - 8].d != turtle.y) {changed = 1;}
            if (ren[len - 7].d != turtle.pensize) {changed = 1;}
            if (ren[len - 6].d != turtle.penr) {changed = 1;}
            if (ren[len - 5].d != turtle.peng) {changed = 1;}
            if (ren[len - 4].d != turtle.penb) {changed = 1;}
            if (ren[len - 3].d != turtle.pena) {changed = 1;}
            if (ren[len - 2].h != turtle.penshape) {changed = 1;}
            if (ren[len - 1].d != turtle.circleprez) {changed = 1;}
        } else {
            changed = 1;
        }
        if (changed == 1) {
            list_append(turtle.penPos, (unitype) turtle.x, 'd');
            list_append(turtle.penPos, (unitype) turtle.y, 'd');
            list_append(turtle.penPos, (unitype) turtle.pensize, 'd');
            list_append(turtle.penPos, (unitype) turtle.penr, 'd');
            list_append(turtle.penPos, (unitype) turtle.peng, 'd');
            list_append(turtle.penPos, (unitype) turtle.penb, 'd');
            list_append(turtle.penPos, (unitype) turtle.pena, 'd');
            list_append(turtle.penPos, (unitype) turtle.penshape, 'h');
            list_append(turtle.penPos, (unitype) turtle.circleprez, 'd');
        }
    }
}
void turtlePenUp() {
    if (turtle.pen == 1) {
        turtle.pen = 0;
        if (turtle.penPos -> length > 0 && turtle.penPos -> type[turtle.penPos -> length - 1] != 'c') {
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
            list_append(turtle.penPos, (unitype) 0, 'c');
        }
    }
}
void turtlePenShape(char *selection) {
    if (strcmp(selection, "circle") == 0 || strcmp(selection, "Circle") == 0) {
        turtle.penshape = 0;
    }
    if (strcmp(selection, "square") == 0 || strcmp(selection, "Square") == 0) {
        turtle.penshape = 1;
    }
    if (strcmp(selection, "triangle") == 0 || strcmp(selection, "Triangle") == 0) {
        turtle.penshape = 2;
    }
    if (strcmp(selection, "none") == 0 || strcmp(selection, "None") == 0) {
        turtle.penshape = 3;
    }
    if (strcmp(selection, "connected") == 0 || strcmp(selection, "Connected") == 0) {
        turtle.penshape = 4;
    }
    if (strcmp(selection, "text") == 0 || strcmp(selection, "Text") == 0) {
        turtle.penshape = 5;
    }
}
void turtlePenPrez(double prez) {
    turtle.circleprez = prez;
}
void turtleGoto(double x, double y) { // moves the turtle to a coordinate
    if (fabs(turtle.x - x) > 0.01 || fabs(turtle.y - y) > 0.01) {
        turtle.x = x;
        turtle.y = y;
        if (turtle.pen == 1) {
            char changed = 0;
            int len = turtle.penPos -> length;
            if (len > 0) {
                unitype *ren = turtle.penPos -> data;
                if (ren[len - 9].d != turtle.x) {changed = 1;}
                if (ren[len - 8].d != turtle.y) {changed = 1;}
                if (ren[len - 7].d != turtle.pensize) {changed = 1;}
                if (ren[len - 6].d != turtle.penr) {changed = 1;}
                if (ren[len - 5].d != turtle.peng) {changed = 1;}
                if (ren[len - 4].d != turtle.penb) {changed = 1;}
                if (ren[len - 3].d != turtle.pena) {changed = 1;}
                if (ren[len - 2].h != turtle.penshape) {changed = 1;}
                if (ren[len - 1].d != turtle.circleprez) {changed = 1;}
            } else {
                changed = 1;
            }
            if (changed == 1) {
                list_append(turtle.penPos, (unitype) x, 'd');
                list_append(turtle.penPos, (unitype) y, 'd');
                list_append(turtle.penPos, (unitype) turtle.pensize, 'd');
                list_append(turtle.penPos, (unitype) turtle.penr, 'd');
                list_append(turtle.penPos, (unitype) turtle.peng, 'd');
                list_append(turtle.penPos, (unitype) turtle.penb, 'd');
                list_append(turtle.penPos, (unitype) turtle.pena, 'd');
                list_append(turtle.penPos, (unitype) turtle.penshape, 'h');
                list_append(turtle.penPos, (unitype) turtle.circleprez, 'd');
            }
        }
    }
}
void turtleCircleRender(double x, double y, double rad, double r, double g, double b, double a, double xfact, double yfact, double prez) { // draws a circle at the specified x and y (coordinates)
    char colorChange = 0;
    if (r != turtle.currentColor[0]) {colorChange = 1;}
    if (g != turtle.currentColor[1]) {colorChange = 1;}
    if (b != turtle.currentColor[2]) {colorChange = 1;}
    if (a != turtle.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtle.currentColor[0] = r;
        turtle.currentColor[1] = g;
        turtle.currentColor[2] = b;
        turtle.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    for (double i = 0; i < prez; i++) {
        glVertex2d((x + rad * sin(2 * i * M_PI / prez)) * xfact, (y + rad * cos(2 * i * M_PI / prez)) * yfact);
    }
    glEnd();
}
void turtleSquareRender(double x1, double y1, double x2, double y2, double r, double g, double b, double a, double xfact, double yfact) { // draws a square
    char colorChange = 0;
    if (r != turtle.currentColor[0]) {colorChange = 1;}
    if (g != turtle.currentColor[1]) {colorChange = 1;}
    if (b != turtle.currentColor[2]) {colorChange = 1;}
    if (a != turtle.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtle.currentColor[0] = r;
        turtle.currentColor[1] = g;
        turtle.currentColor[2] = b;
        turtle.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x1 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y2 * yfact);
    glVertex2d(x1 * xfact, y2 * yfact);
    glEnd();
}
void turtleTriangleRender(double x1, double y1, double x2, double y2, double x3, double y3, double r, double g, double b, double a, double xfact, double yfact) { // draws a triangle
    char colorChange = 0;
    if (r != turtle.currentColor[0]) {colorChange = 1;}
    if (g != turtle.currentColor[1]) {colorChange = 1;}
    if (b != turtle.currentColor[2]) {colorChange = 1;}
    if (a != turtle.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtle.currentColor[0] = r;
        turtle.currentColor[1] = g;
        turtle.currentColor[2] = b;
        turtle.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLES);
    glVertex2d(x1 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y2 * yfact);
    glVertex2d(x3 * xfact, y3 * yfact);
    glEnd();
}
void turtleTriangle(double x1, double y1, double x2, double y2, double x3, double y3, double r, double g, double b, double a) { // adds a (blit) triangle to the pipeline (for better speed)
    list_append(turtle.penPos, (unitype) x1, 'd');
    list_append(turtle.penPos, (unitype) y1, 'd');
    list_append(turtle.penPos, (unitype) x2, 'd');
    list_append(turtle.penPos, (unitype) (r / 255), 'd');
    list_append(turtle.penPos, (unitype) (g / 255), 'd');
    list_append(turtle.penPos, (unitype) (b / 255), 'd');
    list_append(turtle.penPos, (unitype) (a / 255), 'd');
    list_append(turtle.penPos, (unitype) 66, 'h'); // blit triangle signifier
    list_append(turtle.penPos, (unitype) y2, 'd'); // some unconventional formatting but it works

    list_append(turtle.penPos, (unitype) x3, 'd');
    list_append(turtle.penPos, (unitype) y3, 'd');
    list_append(turtle.penPos, (unitype) 0, 'd'); // zero'd out (wasted space)
    list_append(turtle.penPos, (unitype) r, 'd'); // duplicate colour data (wasted space)
    list_append(turtle.penPos, (unitype) g, 'd');
    list_append(turtle.penPos, (unitype) b, 'd');
    list_append(turtle.penPos, (unitype) a, 'd');
    list_append(turtle.penPos, (unitype) 66, 'h'); // blit triangle signifier
    list_append(turtle.penPos, (unitype) 0, 'd'); // zero'd out (wasted space)
}
void turtleQuadRender(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double r, double g, double b, double a, double xfact, double yfact) { // draws a quadrilateral
    char colorChange = 0;
    if (r != turtle.currentColor[0]) {colorChange = 1;}
    if (g != turtle.currentColor[1]) {colorChange = 1;}
    if (b != turtle.currentColor[2]) {colorChange = 1;}
    if (a != turtle.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtle.currentColor[0] = r;
        turtle.currentColor[1] = g;
        turtle.currentColor[2] = b;
        turtle.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x1 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y2 * yfact);
    glVertex2d(x3 * xfact, y3 * yfact);
    glVertex2d(x4 * xfact, y4 * yfact);
    glEnd();
}
void turtleQuad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double r, double g, double b, double a) { // adds a (blit) quad to the pipeline (for better speed)
    list_append(turtle.penPos, (unitype) x1, 'd');
    list_append(turtle.penPos, (unitype) y1, 'd');
    list_append(turtle.penPos, (unitype) x2, 'd');
    list_append(turtle.penPos, (unitype) (r / 255), 'd');
    list_append(turtle.penPos, (unitype) (g / 255), 'd');
    list_append(turtle.penPos, (unitype) (b / 255), 'd');
    list_append(turtle.penPos, (unitype) (a / 255), 'd');
    list_append(turtle.penPos, (unitype) 67, 'h'); // blit quad signifier
    list_append(turtle.penPos, (unitype) y2, 'd'); // some unconventional formatting but it works

    list_append(turtle.penPos, (unitype) x3, 'd');
    list_append(turtle.penPos, (unitype) y3, 'd');
    list_append(turtle.penPos, (unitype) x4, 'd');
    list_append(turtle.penPos, (unitype) r, 'd'); // duplicate colour data (wasted space)
    list_append(turtle.penPos, (unitype) g, 'd');
    list_append(turtle.penPos, (unitype) b, 'd');
    list_append(turtle.penPos, (unitype) a, 'd');
    list_append(turtle.penPos, (unitype) 67, 'h'); // blit quad signifier
    list_append(turtle.penPos, (unitype) y4, 'd');
}
void turtleUpdate() { // draws the turtle's path on the screen
    char changed = 0;
    int len = turtle.penPos -> length;
    unitype *ren = turtle.penPos -> data;
    char *renType = turtle.penPos -> type;
    unitype *lastFrame = turtle.penPosOld -> data;
    if (len != turtle.penPosOld -> length) {
        changed = 1;
    } else {
        for (int i = 0; i < len && changed == 0; i++) {
            switch (renType[i]) {
                case 'd':
                if (ren[i].d != lastFrame[i].d) {changed = 1;}
                break;
                case 'h':
                if (ren[i].h != lastFrame[i].h) {changed = 1;}
                break;
                case 'c':
                if (ren[i].c != lastFrame[i].c) {changed = 1;}
                break;
            }
        }
    }
    glfwGetWindowSize(turtle.window, &turtle.screenbounds[0], &turtle.screenbounds[1]);
    if (turtle.lastscreenbounds[0] != turtle.screenbounds[0] || turtle.lastscreenbounds[1] != turtle.screenbounds[1]) {
        changed = 1;
    }
    turtle.lastscreenbounds[0] = turtle.screenbounds[0];
    turtle.lastscreenbounds[1] = turtle.screenbounds[1];
    list_copy(turtle.penPos, turtle.penPosOld); // unideal
    if (changed == 1) { // only redraw the screen if there have been any changes from last frame
        double xfact = (turtle.bounds[2] - turtle.bounds[0]) / 2;
        double yfact = (turtle.bounds[3] - turtle.bounds[1]) / 2;
        xfact = 1 / xfact;
        yfact = 1 / yfact;
        double lastSize = -1;
        double lastPrez = -1;
        double precomputedLog = 5;
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < len; i += 9) {
            if (renType[i] == 'd') {
                switch (ren[i + 7].h) {
                    case 0:
                    if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                        precomputedLog = ren[i + 8].d * log(2.71 + ren[i + 2].d);
                    lastSize = ren[i + 2].d;
                    lastPrez = ren[i + 8].d;
                    turtleCircleRender(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                    break;
                    case 1:
                    turtleSquareRender(ren[i].d - ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d + ren[i + 2].d, ren[i + 1].d + ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    break;
                    case 2:
                    turtleTriangleRender(ren[i].d - ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d + ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d, ren[i + 1].d + ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    break;
                    case 5:
                    if (i - 9 < 0 || renType[i - 9] == 'c') {
                        if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                            precomputedLog = ren[i + 8].d * log(2.71 + ren[i + 2].d);
                        lastSize = ren[i + 2].d;
                        lastPrez = ren[i + 8].d;
                        turtleCircleRender(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                    }
                    break;
                }
                if (i + 9 < len && renType[i + 9] == 'd' && ren[i + 7].h < 64 && (ren[i + 7].h == 4 || ren[i + 7].h == 5 || (fabs(ren[i].d - ren[i + 9].d) > ren[i + 2].d / 2 || fabs(ren[i + 1].d - ren[i + 10].d) > ren[i + 2].d / 2))) { // tests for next point continuity and also ensures that the next point is at sufficiently different coordinates
                    double dir = atan((ren[i + 9].d - ren[i].d) / (ren[i + 1].d - ren[i + 10].d));
                    double sinn = sin(dir + M_PI / 2);
                    double coss = cos(dir + M_PI / 2);
                    turtleQuadRender(ren[i].d + ren[i + 2].d * sinn, ren[i + 1].d - ren[i + 2].d * coss, ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i].d - ren[i + 2].d * sinn, ren[i + 1].d + ren[i + 2].d * coss, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    if ((ren[i + 7].h == 4 || ren[i + 7].h == 5) && i + 18 < len && renType[i + 18] == 'd') {
                        double dir2 = atan((ren[i + 18].d - ren[i + 9].d) / (ren[i + 10].d - ren[i + 19].d));
                        double sinn2 = sin(dir2 + M_PI / 2);
                        double coss2 = cos(dir2 + M_PI / 2);
                        turtleTriangleRender(ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i + 9].d + ren[i + 11].d * sinn2, ren[i + 10].d - ren[i + 11].d * coss2, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact); // in a perfect world the program would know which one of these triangles to render (to blend the segments)
                        turtleTriangleRender(ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i + 9].d - ren[i + 11].d * sinn2, ren[i + 10].d + ren[i + 11].d * coss2, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact); // however we live in a world where i am bad at math, so it just renders both no matter what (one has no effect)
                    }
                } else {
                    if (ren[i + 7].h == 4 && i > 8 && renType[i - 8] == 'c') {
                        if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                            precomputedLog = ren[i + 8].d * log(2.71 + ren[i + 2].d);
                        lastSize = ren[i + 2].d;
                        lastPrez = ren[i + 8].d;
                        turtleCircleRender(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                    }
                    if (ren[i + 7].h == 5 && i > 8) {
                        if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                            precomputedLog = ren[i + 8].d * log(2.71 + ren[i + 2].d);
                        lastSize = ren[i + 2].d;
                        lastPrez = ren[i + 8].d;
                        turtleCircleRender(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                    }
                }
                if (ren[i + 7].h == 64) { // blit circle

                }
                if (ren[i + 7].h == 66) { // blit triangle
                    turtleTriangleRender(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 8].d, ren[i + 9].d, ren[i + 10].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    i += 9;
                }
                if (ren[i + 7].h == 67) { // blit quad
                    turtleQuadRender(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 8].d, ren[i + 9].d, ren[i + 10].d, ren[i + 11].d, ren[i + 17].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    i += 9;
                }
            }
        }
        glfwSwapBuffers(turtle.window);
    }
    glfwPollEvents();
    if (glfwWindowShouldClose(turtle.window)) {
        turtle.close = 1;
        glfwTerminate();
    }
}
void turtleMainLoop() { // keeps the window open
    while (turtle.close == 0) {
        turtleUpdate();
    }
}

void turtleFree() {
    list_free(turtle.keyPressed);
    list_free(turtle.penPos);
    list_free(turtle.penPosOld);
}
#endif