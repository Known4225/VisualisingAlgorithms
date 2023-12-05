#include "../include/textGL.h"
#include <time.h>

/*
This is for generic graph projects
*/

typedef struct {
    /* Graph stuff */
    list_t *xpos; // visual components
    list_t *ypos;
    list_t *size;
    list_t *shape;
    list_t *text;
    double screenX;
    double screenY;
    double screenSize;
    double mouseX; // mouse position
    double mouseY;
    double focalX;
    double focalY;
    double focalCSX;
    double focalCSY;
    double scrollSpeed;
    double specColor[15]; // colours for start and end
    char keys[12]; // for keybinds
    char selected;
    char selectMode;
    char showDistances; // toggle showing or hiding distance numbers
    char changeDistances; // toggle changing distance relative to total distance
    int wireStart;
    int wireEnd;
    list_t *connections; // AoS - node1, node2, distance
    list_t *adjacentcyList; // alternate representation of graph, contains a list of lists of integers

    /* DFS stuff */
    char directed; // 0 - undirected, 1 - directed
    char completedDFS;
    int current; // currently selected node
    int stepNum;
    int start; // start and end nodes for Graph_t's algorithm
    list_t *visited;
    list_t *completed;
    list_t *backstack;
} Graph_t;

extern inline int randomInt(int lowerBound, int upperBound) { // random integer between lower and upper bound (inclusive)
    return (rand() % (upperBound - lowerBound + 1) + lowerBound);
}

extern inline double randomDouble(double lowerBound, double upperBound) { // random double between lower and upper bound
    return (rand() * (upperBound - lowerBound) / RAND_MAX + lowerBound); // probably works idk
}

void init(Graph_t *selfp, int nodeCount) {
    Graph_t self = *selfp;
    self.directed = 1;
    /* create nodes */
    int numTries = 10000;
    self.xpos = list_init();
    self.ypos = list_init();
    self.size = list_init();
    self.shape = list_init();
    self.text = list_init();
    self.adjacentcyList = list_init();
    self.visited = list_init();
    self.completed = list_init();
    self.backstack = list_init();
    for (int i = 0; i < nodeCount; i++) {
        char num[12];
        sprintf(num, "%d", i);
        list_append(self.xpos, (unitype) randomDouble(-290, 290), 'd');
        list_append(self.ypos, (unitype) randomDouble(-160, 160), 'd');
        list_append(self.size, (unitype) randomDouble((100 / (log(nodeCount + 1) + nodeCount * 0.1)), (150 / (log(nodeCount + 1) + nodeCount * 0.1))), 'd');
        list_append(self.adjacentcyList, (unitype) list_init(), 'r');
        int tries = 1;
        char colliding = 1;
        while (tries < numTries && colliding) { // ensures no two nodes are colliding with each other
            colliding = 0;
            for (int j = 0; j < self.xpos -> length - 1; j++) {
                // printf("%d distance to node %d: %0.2lf\n", self.xpos -> length - 1, j, sqrt((self.xpos -> data[self.xpos -> length - 1].d - self.xpos -> data[j].d) * (self.xpos -> data[self.xpos -> length - 1].d - self.xpos -> data[j].d) + (self.ypos -> data[self.ypos -> length - 1].d - self.ypos -> data[j].d) * (self.ypos -> data[self.ypos -> length - 1].d - self.ypos -> data[j].d)));
                if ((self.xpos -> data[self.xpos -> length - 1].d - self.xpos -> data[j].d) * (self.xpos -> data[self.xpos -> length - 1].d - self.xpos -> data[j].d) + (self.ypos -> data[self.ypos -> length - 1].d - self.ypos -> data[j].d) * (self.ypos -> data[self.ypos -> length - 1].d - self.ypos -> data[j].d) < (self.size -> data[self.xpos -> length - 1].d + self.size -> data[j].d) * (self.size -> data[self.xpos -> length - 1].d + self.size -> data[j].d) / 4) {
                    colliding = 1;
                    self.xpos -> data[self.xpos -> length - 1] = (unitype) randomDouble(-290, 290);
                    self.ypos -> data[self.ypos -> length - 1] = (unitype) randomDouble(-160, 160);
                    // printf("error: %d collides with %d\n", self.xpos -> length - 1, j);
                    break;
                }
            }
            tries += 1;
        }
        if (tries >= numTries) {
            numTries /= 2;
            printf("collision\n");
            if (numTries < 1) {
                printf("maxed collisions\n");
                numTries = 1;
            }
        }
        list_append(self.shape, (unitype) 0, 'c'); // 0 - circle, 1 - square, 2 - rectangle
        list_append(self.text, (unitype) num, 's');
    }

    /* create connections */
    list_t *rankedDistances = list_init();
    self.connections = list_init();
    for (int totalTries = 0; totalTries < nodeCount * nodeCount / 4; totalTries++) { // the total number of attempted connections is nodeCount squared upon 4
        int i = randomInt(0, nodeCount - 1); // this works by randomly generating pairs and putting a connection between them (if eligible)
        // int j = randomInt(0, nodeCount - 1); // since connections made earlier take precedent over those made later, this made it more spread between nodes (otherwise 0 would get all the connections it could which robs other nodes)
        // this all works great, but the last thing I would like to do is to prefer connections between nodes that are closer since this will generate more human looking graphs, so instead of chosing two nodes randomly I'll choose one node randomly and choose the second node with a distance heuristic
        list_clear(rankedDistances);
        int min;
        double minDist;
        for (int m = 0; m < self.xpos -> length - 1; m++) {
            min = 0;
            minDist = 1000000000000.0; // too scared to use a max float macro
            for (int k = 0; k < self.xpos -> length; k++) {
                if ((self.xpos -> data[i].d - self.xpos -> data[k].d) * (self.xpos -> data[i].d - self.xpos -> data[k].d) + (self.ypos -> data[i].d - self.ypos -> data[k].d) * (self.ypos -> data[i].d - self.ypos -> data[k].d) < minDist && i != k && list_count(rankedDistances, (unitype) k, 'i') < 1) {
                    min = k;
                    minDist = (self.xpos -> data[i].d - self.xpos -> data[k].d) * (self.xpos -> data[i].d - self.xpos -> data[k].d) + (self.ypos -> data[i].d - self.ypos -> data[k].d) * (self.ypos -> data[i].d - self.ypos -> data[k].d);
                }
            }
            list_append(rankedDistances, (unitype) min, 'i');
        }
        // printf("for node %d: ", i);
        // list_print(rankedDistances);
        int j = rankedDistances -> data[0].i;
        for (int k = 0; k < rankedDistances -> length; k++) { // successive random choices starting with the closest to the furthest
            if (randomInt(0, (int) sqrt(nodeCount)) < 1) {
                j = rankedDistances -> data[k].i;
                break;
            }
        }
        
        // idea: algorithm that maximises the number of connections a graph can have and remain planar (actually not remain planar but i mean like you have to connect with straight lines and they cant cross and you also cant move the nodes)
        // I know a greedy algorithm would be VERY bad for this particular problem
        if (i != j) {
            char eligible = 0;
            double x1, y1, x2, y2;
            if (self.xpos -> data[i].d > self.xpos -> data[j].d) { // ensures x1, y1 is the leftmost pair
                x1 = self.xpos -> data[j].d;
                y1 = self.ypos -> data[j].d;
                x2 = self.xpos -> data[i].d;
                y2 = self.ypos -> data[i].d;
            } else {
                x1 = self.xpos -> data[i].d;
                y1 = self.ypos -> data[i].d;
                x2 = self.xpos -> data[j].d;
                y2 = self.ypos -> data[j].d;
            }
            double slope1 = (y1 - y2) / (x1 - x2);
            double ycept1 = -slope1 * x1 + y1;
            for (int k = 0; k < self.connections -> length; k += 3) {
                if (self.directed) {
                    if (self.connections -> data[k].i == i && self.connections -> data[k + 1].i == j) { // check if the connection has already been made to avoid duplicate connections
                        eligible = 1;
                        // printf("error: duplicate %d to %d\n", i, j);
                        break;
                    }
                } else {
                    if ((self.connections -> data[k].i == i && self.connections -> data[k + 1].i == j) || (self.connections -> data[k].i == j && self.connections -> data[k + 1].i == i)) { // check if the connection has already been made to avoid duplicate connections
                        eligible = 1;
                        // printf("error: duplicate %d to %d\n", i, j);
                        break;
                    }
                }
                double x3, y3, x4, y4;
                if (self.xpos -> data[self.connections -> data[k].i].d > self.xpos -> data[self.connections -> data[k + 1].i].d) { // ensures x3, y3 is the leftmost pair
                    x3 = self.xpos -> data[self.connections -> data[k + 1].i].d;
                    y3 = self.ypos -> data[self.connections -> data[k + 1].i].d;
                    x4 = self.xpos -> data[self.connections -> data[k].i].d;
                    y4 = self.ypos -> data[self.connections -> data[k].i].d;
                } else {
                    x3 = self.xpos -> data[self.connections -> data[k].i].d;
                    y3 = self.ypos -> data[self.connections -> data[k].i].d;
                    x4 = self.xpos -> data[self.connections -> data[k + 1].i].d;
                    y4 = self.ypos -> data[self.connections -> data[k + 1].i].d;
                }
                double slope2 = (y3 - y4) / (x3 - x4);
                double ycept2 = -slope2 * x3 + y3;
                double interceptX = (ycept2 - ycept1) / (slope1 - slope2);
                if (interceptX - 0.01 > x1 && interceptX + 0.01 < x2 && interceptX - 0.01 > x3 && interceptX + 0.01 < x4) { // check if this connection would cross another connection, to ensure the graph is planar
                    eligible = 1;
                    // printf("error: crossing from %d to %d crosses %d to %d with xcept %0.2lf\n", i, j, self.connections -> data[k].i, self.connections -> data[k + 1].i, interceptX);
                    break;
                }
            }
            if (!eligible) {
                list_append(self.connections, (unitype) i, 'i'); // adds i and j after, signifying that they are connected
                list_append(self.connections, (unitype) j, 'i');
                if (self.directed) {
                    for (int h = 0; h < self.connections -> length; h += 3) {
                        if (self.connections -> data[h].i == j && self.connections -> data[h + 1].i == i) {
                            list_append(self.connections, self.connections -> data[h + 2], 'd');
                            eligible = 1;
                            break;
                        }
                    }
                }
                if (!eligible) {
                    list_append(self.connections, (unitype) randomDouble(4, 16), 'd'); // distance (weight value)
                }
                list_append(self.adjacentcyList -> data[i].r, (unitype) j, 'i'); // update adjacentcy list
                if (self.directed == 0) {
                    list_append(self.adjacentcyList -> data[j].r, (unitype) i, 'i'); // if not directed then do both
                }
            }
        }
    }
    list_free(rankedDistances);

    /* general */
    double colors[15] = {38, 235, 242, 38, 235, 242, 116, 255, 133, 19, 236, 48, 255, 255, 255};
    memcpy(self.specColor, colors, sizeof(double) * 15);
    for (int i = 0; i < 8; i++) {
        self.keys[i] = 0;
    }
    self.selectMode = 0;
    self.selected = -1;
    self.mouseX = 0;
    self.mouseY = 0;
    self.wireStart = -1;
    self.wireEnd = -1;
    self.screenX = 0;
    self.screenY = 0;
    self.screenSize = 1;
    self.focalX = 0;
    self.focalY = 0;
    self.focalCSX = 0;
    self.focalCSY = 0;
    self.scrollSpeed = 1.15;
    self.wireStart = -1;
    self.showDistances = 0;
    self.changeDistances = 0;
    self.completedDFS = 0;

    /* Graph_t */
    self.start = -1;
    self.stepNum = 0;
    *selfp = self;
}

void setupDepth(Graph_t *selfp) {
    Graph_t self = *selfp;
    if (self.start != -1) {
        self.completedDFS = 0;
        self.stepNum = 1;
        list_append(self.backstack, (unitype) self.start, 'i'); // add to call stack
        self.current = self.start;
    }
    *selfp = self;
}

void stepDepth(Graph_t *selfp) {
    Graph_t self = *selfp;
    if (self.stepNum > 0 && self.completedDFS == 0) {
        self.current = self.backstack -> data[self.backstack -> length - 1].i;
        self.stepNum += 1;
        list_print(self.backstack);
        printf("step %d\n", self.current);
        for (int i = 0; i < self.visited -> length; i++) {
            if (self.visited -> data[i].i == self.current) {
                if (self.backstack -> length > 0) {
                    printf("removed %d\n", self.backstack -> data[self.backstack -> length - 1].i);
                    list_append(self.completed, self.backstack -> data[self.backstack -> length - 1], 'i');
                    list_pop(self.backstack); // remove from call stack
                }
                if (self.backstack -> length == 0) {
                    printf("completed\n");
                    self.completedDFS = 1;
                }
                *selfp = self;
                return;
            }
        }
        
        list_append(self.visited, (unitype) self.current, 'i'); // add current node to visited
        for (int i = 0; i < self.adjacentcyList -> data[self.current].r -> length; i++) {
            char found = 0;
            for (int j = 0; j < self.visited -> length; j++) {
                if (self.visited -> data[j].i == self.adjacentcyList -> data[self.current].r -> data[i].i) {
                    found = 1;
                    break;
                }
            }
            if (!found)
                list_append(self.backstack, (unitype) self.adjacentcyList -> data[self.current].r -> data[i].i, 'i'); // add to call stack
        }
    }
    *selfp = self;
}

void renderGraph(Graph_t *selfp) { // renders the nodes
    Graph_t self = *selfp;
    for (int i = self.xpos -> length - 1; i > -1; i--) {
        switch (self.shape -> data[i].c) {
            case 0:
            turtlePenShape("circle");
            break;
            case 1:
            turtlePenShape("square");
            break;
            case 2:
            turtlePenShape("square");
            break;
            default:
            turtlePenShape("circle");
        }
        if (self.selected == i) {
            turtleGoto((self.xpos -> data[i].d + self.screenX) * self.screenSize, (self.ypos -> data[i].d + self.screenY) * self.screenSize);
            turtlePenColor(120, 120, 120);
            turtlePenSize(self.size -> data[i].d * 1.1 * self.screenSize);
            turtlePenDown();
            turtlePenUp();
        }
        if (self.start == i) {
            turtlePenColor(self.specColor[0], self.specColor[1], self.specColor[2]);
        } else {
            turtlePenColor(100.0, 100.0, 100.0);
        }
        for (int j = 0; j < self.visited -> length; j += 1) {
            if (i == self.visited -> data[j].i) {
                turtlePenColor(self.specColor[6], self.specColor[7], self.specColor[8]);
                break;
            }
        }
        for (int j = 0; j < self.completed -> length; j += 1) {
            if (i == self.completed -> data[j].i) {
                turtlePenColor(self.specColor[9], self.specColor[10], self.specColor[11]);
                break;
            }
        }
        turtlePenSize(self.size -> data[i].d * self.screenSize);
        turtleGoto((self.xpos -> data[i].d + self.screenX) * self.screenSize, (self.ypos -> data[i].d + self.screenY) * self.screenSize);
        turtlePenDown();
        turtlePenUp();
        turtlePenColor(30, 30, 30);
        textGLWriteString(self.text -> data[i].s, (self.xpos -> data[i].d + self.screenX) * self.screenSize, (self.ypos -> data[i].d + self.screenY) * self.screenSize, self.size -> data[i].d * 0.6 * self.screenSize, 50);
    }
    turtlePenShape("circle");
    // *selfp = self; // no need to restoring
}

void renderConnections(Graph_t *selfp) { // renders the connections between nodes
    Graph_t self = *selfp;
    turtlePenSize(self.size -> data[self.connections -> data[0].i].d * 0.07 * self.screenSize);
    for (int i = 0; i < self.connections -> length; i += 3) {
        turtlePenColor(60, 60, 60);
        if (list_count(self.visited, (unitype) self.connections -> data[i].i, 'i') && list_count(self.visited, (unitype) self.connections -> data[i + 1].i, 'i')) {
            turtlePenColor(self.specColor[6], self.specColor[7], self.specColor[8]);
        }
        if (list_count(self.completed, (unitype) self.connections -> data[i].i, 'i') && list_count(self.completed, (unitype) self.connections -> data[i + 1].i, 'i')) {
            turtlePenColor(self.specColor[9], self.specColor[10], self.specColor[11]);
        }
        turtleGoto((self.xpos -> data[self.connections -> data[i].i].d + self.screenX) * self.screenSize, (self.ypos -> data[self.connections -> data[i].i].d + self.screenY) * self.screenSize);
        turtlePenDown();
        if (self.directed == 1) {
            double x1 = (self.xpos -> data[self.connections -> data[i].i].d + self.screenX) * self.screenSize;
            double y1 = (self.ypos -> data[self.connections -> data[i].i].d + self.screenY) * self.screenSize;
            double x2 = (self.xpos -> data[self.connections -> data[i + 1].i].d + self.screenX) * self.screenSize;
            double y2 = (self.ypos -> data[self.connections -> data[i + 1].i].d + self.screenY) * self.screenSize;
            double theta = atan((y2 - y1) / (x2 - x1));
            if (x2 > x1) {
                theta += 3.141592;
            }
            double destSize = (self.size -> data[self.connections -> data[i + 1].i].d - 2);
            x1 = x2 + cos(theta) * destSize * self.screenSize;
            y1 = y2 + sin(theta) * destSize * self.screenSize;
            turtleGoto(x1, y1);
            theta += 0.785398; // 45 degrees
            turtleGoto(x1 + cos(theta) * destSize * 0.5 * self.screenSize, y1 + sin(theta) * destSize * 0.5 * self.screenSize);
            theta -= 0.785398 * 2;
            turtlePenUp();
            turtleGoto(x1, y1);
            turtlePenDown();
            turtleGoto(x1 + cos(theta) * destSize * 0.5 * self.screenSize, y1 + sin(theta) * destSize * 0.5 * self.screenSize);
        } else {
            turtleGoto((self.xpos -> data[self.connections -> data[i + 1].i].d + self.screenX) * self.screenSize, (self.ypos -> data[self.connections -> data[i + 1].i].d + self.screenY) * self.screenSize);
        }
        turtlePenUp();
    }
    if (self.selectMode == 4) {
        turtleGoto((self.xpos -> data[self.wireStart].d + self.screenX) * self.screenSize, (self.ypos -> data[self.wireStart].d + self.screenY) * self.screenSize);
        turtlePenDown();
        if (self.wireEnd == -1) {
            turtleGoto(self.mouseX, self.mouseY);
        } else {
            turtleGoto((self.xpos -> data[self.wireEnd].d + self.screenX) * self.screenSize, (self.ypos -> data[self.wireEnd].d + self.screenY) * self.screenSize);
        }
        turtlePenUp();
    }
    *selfp = self;
}

void renderConnectionLabels(Graph_t *selfp) { // renders numberic labels for the connections
    Graph_t self = *selfp;
    for (int i = 0; i < self.connections -> length; i += 3) {
        double writeX = ((self.xpos -> data[self.connections -> data[i].i].d + self.screenX) + (self.xpos -> data[self.connections -> data[i + 1].i].d + self.screenX)) / 2 * self.screenSize ;
        double writeY = ((self.ypos -> data[self.connections -> data[i].i].d + self.screenY) + (self.ypos -> data[self.connections -> data[i + 1].i].d + self.screenY)) / 2 * self.screenSize;
        char num[10];
        if (self.changeDistances) {
            sprintf(num, "%.0lf", self.connections -> data[i + 2].d * sqrt((self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) * 
                                                                                                  (self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) + 
                                                                                                  (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d) * 
                                                                                                  (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d)));
        } else {
            sprintf(num, "%.0lf", self.connections -> data[i + 2].d);
        }
        turtlePenColor(0, 0, 0);
        textGLWriteString(num, writeX, writeY, self.size -> data[self.connections -> data[0].i].d * 0.4 * self.screenSize, 50);
    }
    *selfp = self; // just for safety
}

extern inline double dmod(double a, double modulus) { // always positive mod
    double out = fmod(a, modulus);
    if (out < 0) {
        return modulus + out;
    }
    return out;
}

void snapToGrid(Graph_t *selfp, double gridSize) { // the classic n^2 snap to grid algorithm, one day I'm going to solve this in O(n) you just wait, and it will be coke milk
    Graph_t self = *selfp;
    int modMedX = 0;
    int modMedY = 0;
    double minDistX = 1000000000.0;
    double minDistY = 1000000000.0;
    for (int i = 0; i < self.xpos -> length; i++) {
    /* the theorem states that for any set of real numbers, there is one that is at the minimum distance mod n from all the other nodes, known as the modular median,
       this n^2 algorithm finds the modular median, but it is conjectured it could be done in O(n) like a regular median can */
        double accDistX = 0;
        double accDistY = 0;
        for (int j = 0; j < self.xpos -> length; j++) {
            accDistX += dmod(self.xpos -> data[j].d - self.xpos -> data[i].d, gridSize);
            accDistY += dmod(self.ypos -> data[j].d - self.ypos -> data[i].d, gridSize);
        }
        if (accDistX < minDistX) {
            modMedX = i;
            minDistX = accDistX;
        }
        if (accDistY < minDistY) {
            modMedY = i;
            minDistY = accDistY;
        }
    }
    //printf("modMedX: %d, modMedY = %d\n", modMedX, modMedY);
    self.screenX += self.xpos -> data[modMedX].d;
    self.screenY += self.ypos -> data[modMedY].d;
    double changeX = self.xpos -> data[modMedX].d;
    double changeY = self.ypos -> data[modMedY].d;
    for (int i = 0; i < self.xpos -> length; i++) { // move all the nodes
        self.xpos -> data[i].d -= changeX;
        self.ypos -> data[i].d -= changeY;
    }
    for (int i = 0; i < self.xpos -> length; i++) { // center on 0
        double modX = dmod(self.xpos -> data[i].d, gridSize);
        double modY = dmod(self.ypos -> data[i].d, gridSize);
        if (modX > gridSize / 2) {
            self.xpos -> data[i].d += gridSize - modX;
        } else {
            self.xpos -> data[i].d -= modX;
        }
        if (modY > gridSize / 2) {
            self.ypos -> data[i].d += gridSize - modY;
        } else {
            self.ypos -> data[i].d -= modY;
        }
    }
    *selfp = self;
}
 
void mouseTick(Graph_t *selfp) {
    Graph_t self = *selfp;
    turtleGetMouseCoords(); // get the mouse coordinates (turtle.mouseX, turtle.mouseY)
    self.mouseX = turtle.mouseX;
    self.mouseY = turtle.mouseY;
    
    if (self.selectMode == 0 || self.selectMode == 4) {
        self.selected = -1;
        self.wireEnd = -1;
        for (int i = 0; i < self.xpos -> length; i++) {
            if ((self.mouseX - (self.xpos -> data[i].d + self.screenX) * self.screenSize) * 
                (self.mouseX - (self.xpos -> data[i].d + self.screenX) * self.screenSize) + 
                (self.mouseY - (self.ypos -> data[i].d + self.screenY) * self.screenSize) * 
                (self.mouseY - (self.ypos -> data[i].d + self.screenY) * self.screenSize) < 
                self.size -> data[i].d * self.size -> data[i].d * 0.25 * self.screenSize * self.screenSize) {
                if (self.selectMode == 0) {
                    self.selected = i;
                } else {
                    self.wireEnd = i;
                }
                break;
            }
        }
    }
    if (turtleMouseDown()) {
        if (self.keys[0] == 0) {
            self.keys[0] = 1;
            if (self.selected == -1) {
                self.selectMode = 0;
            } else {
                if (self.selectMode == 2) {
                    self.selectMode = 3;
                    self.selected = -1;
                } else {
                    self.selectMode = 1;
                }
            }
            if (self.selectMode == 1) {
                if (turtleKeyPressed(GLFW_KEY_LEFT_SHIFT) || turtleKeyPressed(GLFW_KEY_SPACE)) {
                    self.wireStart = self.selected;
                    self.selectMode = 4;
                } else {
                    self.focalCSX = self.xpos -> data[self.selected].d;
                    self.focalCSY = self.ypos -> data[self.selected].d;
                    self.focalX = self.mouseX - (self.focalCSX + self.screenX) * self.screenSize;
                    self.focalY = self.mouseY - (self.focalCSY + self.screenY) * self.screenSize;
                }
            } else {
                if (self.selectMode == 0) {
                    self.focalX = self.mouseX;
                    self.focalY = self.mouseY;
                    self.focalCSX = self.screenX;
                    self.focalCSY = self.screenY;
                }
            } 
        } else {
            if (self.selectMode == 1) {
                self.xpos -> data[self.selected] = (unitype) ((self.mouseX - self.focalX) / self.screenSize - self.screenX);
                self.ypos -> data[self.selected] = (unitype) ((self.mouseY - self.focalY) / self.screenSize - self.screenY);
            } else {
                if (self.selectMode == 0) {
                    self.screenX = (self.mouseX - self.focalX) / self.screenSize + self.focalCSX;
                    self.screenY = (self.mouseY - self.focalY) / self.screenSize + self.focalCSY;
                }
            }
        }
    } else {
        if (self.keys[0] == 1) {
            if (self.selectMode == 4 && self.wireStart != -1 && self.wireEnd != -1) {
                char found = 0;
                for (int i = 0; i < self.connections -> length; i += 3) {
                    if (self.directed) {
                        if (self.connections -> data[i].i == self.wireStart && self.connections -> data[i + 1].i == self.wireEnd) {
                            list_delete(self.connections, i);
                            list_delete(self.connections, i);
                            list_delete(self.connections, i);
                            for (int j = 0; j < self.adjacentcyList -> data[self.wireStart].r -> length; j++) {
                                if (self.adjacentcyList -> data[self.wireStart].r -> data[j].i == self.wireEnd) {
                                    list_delete(self.adjacentcyList -> data[self.wireStart].r, j);
                                    break;
                                }
                            }
                            found = 1;
                            break;
                        }
                    } else {
                        if ((self.connections -> data[i].i == self.wireStart && self.connections -> data[i + 1].i == self.wireEnd) || (self.connections -> data[i + 1].i == self.wireStart && self.connections -> data[i].i == self.wireEnd)) {
                            list_delete(self.connections, i);
                            list_delete(self.connections, i);
                            list_delete(self.connections, i);
                            for (int j = 0; j < self.adjacentcyList -> data[self.wireStart].r -> length; j++) {
                                if (self.adjacentcyList -> data[self.wireStart].r -> data[j].i == self.wireEnd) {
                                    list_delete(self.adjacentcyList -> data[self.wireStart].r, j);
                                    break;
                                }
                            }
                            for (int j = 0; j < self.adjacentcyList -> data[self.wireEnd].r -> length; j++) {
                                if (self.adjacentcyList -> data[self.wireEnd].r -> data[j].i == self.wireStart) {
                                    list_delete(self.adjacentcyList -> data[self.wireEnd].r, j);
                                    break;
                                }
                            }
                            found = 1;
                            break;
                        }
                    }
                }
                if (!found) {
                    list_append(self.connections, (unitype) self.wireStart, 'i'); // add a new connection
                    list_append(self.connections, (unitype) self.wireEnd, 'i');
                    if (self.directed) {
                        for (int i = 0; i < self.connections -> length; i += 3) {
                            if (self.connections -> data[i + 1].i == self.wireStart && self.connections -> data[i].i == self.wireEnd) {
                                list_append(self.connections, self.connections -> data[i + 2], 'd');
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (!found) {
                        list_append(self.connections, (unitype) randomDouble(4, 16), 'd');
                    }
                    list_append(self.adjacentcyList -> data[self.wireStart].r, (unitype) self.wireEnd, 'i'); // update adjacentcy list
                    if (self.directed == 0) {
                        list_append(self.adjacentcyList -> data[self.wireEnd].r, (unitype) self.wireStart, 'i'); // if not directed then do both
                    }
                }
            }
            if (self.selected != -1 && fabs(self.xpos -> data[self.selected].d - self.focalCSX) < 0.01 && fabs(self.ypos -> data[self.selected].d - self.focalCSY) < 0.01) {
                self.start = self.selected;
                setupDepth(&self);
            }
            self.selectMode = 0;
            self.keys[0] = 0;
        }
        if (self.selectMode == 2) {
            self.xpos -> data[self.selected] = (unitype) ((self.mouseX - self.focalX) / self.screenSize - self.screenX);
            self.ypos -> data[self.selected] = (unitype) ((self.mouseY - self.focalY) / self.screenSize - self.screenY);
        }
    }
    *selfp = self;
}

void scrollTick(Graph_t *selfp) {
    Graph_t self = *selfp;
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

void hotkeyTick(Graph_t *selfp) {
    Graph_t self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE)) {
        if (self.keys[1] == 0) {
            self.keys[1] = 1;
            stepDepth(&self);
        } else {
            self.keys[9] += 1;
            if (self.keys[9] > 30) {
                self.keys[1] = 0;
                self.keys[9] = 31;
            }
        }
    } else {
        self.keys[1] = 0;
        self.keys[9] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_Z)) { // Z - snap to grid
        if (self.keys[2] == 0) {
            self.keys[2] = 1;
            snapToGrid(&self, (110 / log(self.xpos -> length + 1)));
        }
    } else {
        self.keys[2] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_C)) { // C - clear all nodes
        if (self.keys[3] == 0) {
            self.keys[3] = 1;
            self.start = -1;
            self.selected = -1;
            self.selectMode = 0;
            self.completedDFS = 0;
            list_clear(self.xpos);
            list_clear(self.ypos);
            list_clear(self.size);
            list_clear(self.shape);
            list_clear(self.text);
            list_clear(self.connections);
            list_clear(self.adjacentcyList);
            list_clear(self.visited);
            list_clear(self.completed);
            list_clear(self.backstack);
        }
    } else {
        self.keys[3] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_R)) { // R - reset Graph_t
        if (self.keys[4] == 0) {
            list_clear(self.visited);
            list_clear(self.completed);
            list_clear(self.backstack);
            self.completedDFS = 0;
            self.keys[4] = 1;
            self.start = -1;
            self.selected = -1;
            self.selectMode = 0;
            self.stepNum = 0;
        }
    } else {
        self.keys[4] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_X)) {
        if (self.keys[5] == 0) {
            self.keys[5] = 1;
            if (self.selected != -1 && self.stepNum == 0) {
                list_delete(self.xpos, self.selected);
                list_delete(self.ypos, self.selected);
                list_delete(self.size, self.selected);
                list_delete(self.shape, self.selected);
                list_delete(self.text, self.selected);
                for (int i = 0; i < self.connections -> length; i += 3) {
                    if (self.connections -> data[i].i == self.selected || self.connections -> data[i + 1].i == self.selected) {
                        list_delete(self.connections, i);
                        list_delete(self.connections, i);
                        list_delete(self.connections, i);
                        i -= 3;
                    } else {
                        if (self.connections -> data[i].i > self.selected) {
                            self.connections -> data[i].i -= 1;
                        }
                        if (self.connections -> data[i + 1].i > self.selected) {
                            self.connections -> data[i + 1].i -= 1;
                        }
                    }
                }
                for (int i = 0; i < self.adjacentcyList -> length; i++) {
                    for (int j = 0; j < self.adjacentcyList -> data[i].r -> length; j++) {
                        if (self.adjacentcyList -> data[i].r -> data[j].i > self.selected) {
                            self.adjacentcyList -> data[i].r -> data[j].i -= 1;
                        }
                    }
                }
                list_delete(self.adjacentcyList, self.selected);
                for (int i = self.selected; i < self.xpos -> length; i++) {
                    char num[12];
                    sprintf(num, "%d", i);
                    self.text -> data[i].s = strdup(num);
                }
                self.selected = -1;
            }
            list_print(self.adjacentcyList);
        }
    } else {
        self.keys[5] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_1)) {
        if (self.keys[6] == 0) {
            self.keys[6] = 1;
            char num[12];
            sprintf(num, "%d", self.xpos -> length);
            self.selected = self.xpos -> length;
            self.selectMode = 2;
            list_append(self.xpos, (unitype) (self.mouseX / self.screenSize - self.screenX), 'd');
            list_append(self.ypos, (unitype) (self.mouseY / self.screenSize - self.screenY), 'd');
            list_append(self.size, (unitype) randomDouble((100 / log(150)), (150 / log(150))), 'd');
            list_append(self.shape, (unitype) 0, 'c'); // 0 - circle, 1 - square, 2 - rectangle
            list_append(self.text, (unitype) num, 's');
            list_append(self.adjacentcyList, (unitype) list_init(), 'r');
            self.focalCSX = self.xpos -> data[self.selected].d;
            self.focalCSY = self.ypos -> data[self.selected].d;
            self.focalX = self.mouseX - (self.focalCSX + self.screenX) * self.screenSize;
            self.focalY = self.mouseY - (self.focalCSY + self.screenY) * self.screenSize;
        }
    } else {
        self.keys[6] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_E)) { // toggle show distance
        if (self.keys[7] == 0) {
            self.keys[7] = 1;
            if (self.showDistances) {
                self.showDistances = 0;
            } else {
                self.showDistances = 1;
            }
        }
    } else {
        self.keys[7] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_D)) {
        if (self.keys[8] == 0) {
            self.keys[8] = 1;
            if (self.changeDistances) {
                self.changeDistances = 0;
                for (int i = 0; i < self.connections -> length; i += 3) {
                    self.connections -> data[i + 2].d = (self.connections -> data[i + 2].d * sqrt((self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) * 
                                                                                                  (self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) + 
                                                                                                  (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d) * 
                                                                                                  (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d)));
                }
            } else {
                self.changeDistances = 1;
                for (int i = 0; i < self.connections -> length; i += 3) {
                    // printf("%lf\n", sqrt((self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) * 
                    //                      (self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) + 
                    //                      (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d) * 
                    //                      (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d)));
                    self.connections -> data[i + 2].d = (self.connections -> data[i + 2].d / sqrt((self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) * 
                                                                                                  (self.xpos -> data[self.connections -> data[i].i].d - self.xpos -> data[self.connections -> data[i + 1].i].d) + 
                                                                                                  (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d) * 
                                                                                                  (self.ypos -> data[self.connections -> data[i].i].d - self.ypos -> data[self.connections -> data[i + 1].i].d)));
                }
            }
        }
    } else {
        self.keys[8] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_Q)) {
        if (self.keys[10] == 0) {
            self.keys[10] = 1;
            init(&self, self.xpos -> length);
        }
    } else {
        self.keys[10] = 0;
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
    window = glfwCreateWindow(1280, 720, "Depth First Search", NULL, NULL);
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
    Graph_t obj; // principle object
    srand(time(NULL)); // randomiser init seed
    if (argc == 1) {
        init(&obj, 20);
    } else {
        int nodeCount = 0;
        sscanf(argv[1], "%d\n", &nodeCount);
        init(&obj, nodeCount);
    }
    
    while (turtle.close == 0) { // main loop
        start = clock();
        turtleClear();
        mouseTick(&obj);
        scrollTick(&obj);
        hotkeyTick(&obj);
        renderConnections(&obj);
        if (obj.showDistances) {
            renderConnectionLabels(&obj);
        }
        renderGraph(&obj);

        turtleUpdate(); // update the screen
        end = clock();
        // printf("ms: %d\n", end - start);
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}