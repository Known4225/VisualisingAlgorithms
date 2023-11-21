#include "../include/textGL.h"
#include <time.h>

typedef struct {
    int ID; // ID, element # in main list
    double xpos;
    double ypos;
    double size;
    double color[3]; // colour data
    char *text; // text data for display
    list_t *neighbors; // a list of IDs of the other elements
} Node;

typedef struct {
    char keys[8]; // for keybinds
    char selectMode;
    char showDistances;
    char finishedDijkstra;
    char toggle;
    int nodeCount;
    int selected;
    int start; // start and end nodes for dijkstra's algorithm
    int end;
    int stepNum;
    int wireStart;
    int wireEnd;
    double mouseX;
    double mouseY;
    double screenX;
    double screenY;
    double globalsize;
    double focalX;
    double focalY;
    double focalCSX;
    double focalCSY;
    double scrollSpeed;
    double specColor[15]; // colours for start and end
    list_t *nodes; // list of node pointers
    list_t *connections; // list of integer IDs
    list_t *completed; // list of completed nodes
    list_t *queue; // priority queue of unconquered nodes
} Dijkstra;

extern inline int randomInt(int lowerBound, int upperBound) { // random integer between lower and upper bound (inclusive)
    return (rand() % (upperBound - lowerBound + 1) + lowerBound);
}

extern inline double randomDouble(double lowerBound, double upperBound) { // random double between lower and upper bound
    return (rand() * (upperBound - lowerBound) / RAND_MAX + lowerBound); // probably works idk
}

Node *newNode(int ID, double xpos, double ypos, double size, double r, double g, double b, char *text) {
    Node *create = malloc(sizeof(Node));
    create -> ID = ID;
    create -> xpos = xpos;
    create -> ypos = ypos;
    create -> size = size;
    create -> color[0] = r;
    create -> color[1] = g;
    create -> color[2] = b;
    create -> text = malloc(strlen(text) + 1);
    memcpy(create -> text, text, strlen(text) + 1);
    create -> neighbors = list_init();
    return create;
}

void init(Dijkstra *selfp, int nodeCount) {
    Dijkstra self = *selfp;
    /* create nodes */
    self.nodeCount = nodeCount;
    self.nodes = list_init();
    for (int i = 0; i < nodeCount; i++) {
        char num[12];
        sprintf(num, "%d", i);
        list_append(self.nodes, (unitype) (void *) newNode(i, randomDouble(-290, 290), randomDouble(-160, 160), randomDouble(20, 50), 100, 100, 100, num), 'p'); // adds pointers to the nodes
    }
    /* create connections */
    self.connections = list_init();
    for (int i = 0; i < nodeCount; i++) {
        for (int j = 0; j < nodeCount; j++) {
            if (randomInt(0, nodeCount) < 1 && i != j && list_count(((Node *) self.nodes -> data[i].p) -> neighbors, (unitype) j, 'i') < 1) { // 1 / nodeCount chance to create a connection with each node
                list_append(self.connections, (unitype) i, 'i'); // adds i and j after, signifying that they are connected
                list_append(self.connections, (unitype) j, 'i');
                list_append(self.connections, (unitype) 0.0, 'd'); // distance between nodes
                list_append(((Node *) self.nodes -> data[i].p) -> neighbors, (unitype) j, 'i'); // adds the integer of j to i's neighbor list
                list_append(((Node *) self.nodes -> data[i].p) -> neighbors, (unitype) 0.0, 'd'); // adds the distance from j to i
                list_append(((Node *) self.nodes -> data[j].p) -> neighbors, (unitype) i, 'i');
                list_append(((Node *) self.nodes -> data[j].p) -> neighbors, (unitype) 0.0, 'd');
            }
        }
    }

    /* general */
    for (int i = 0; i < 8; i++) {
        self.keys[i] = 0;
    }
    self.selectMode = 0;
    self.selected = -1;
    self.showDistances = 0;
    self.toggle = 0;
    self.start = -1;
    self.end = -1;
    self.wireStart = -1;
    self.wireEnd = -1;
    self.finishedDijkstra = 0;
    self.stepNum = 0;
    self.mouseX = 0;
    self.mouseY = 0;
    self.screenX = 0;
    self.screenY = 0;
    self.globalsize = 1;
    self.focalX = 0;
    self.focalY = 0;
    self.focalCSX = 0;
    self.focalCSY = 0;
    self.scrollSpeed = 1.15;
    double colors[15] = {38, 235, 242, 38, 235, 242, 116, 255, 133, 19, 236, 48, 255, 255, 255};
    memcpy(self.specColor, colors, sizeof(double) * 15);
    

    /* Dijkstra */
    self.completed = list_init();
    self.queue = list_init();

    *selfp = self;
}

void setupDijkstra(Dijkstra *selfp) {
    Dijkstra self = *selfp;
    if (self.start != -1 && self.end != -1) {
        self.stepNum = 0;
        self.finishedDijkstra = 0;
        list_clear(self.completed);
        list_clear(self.queue);
        list_append(self.queue, (unitype) self.start, 'i'); // node ID
        list_append(self.queue, (unitype) 0.0, 'd'); // shortest known distance from start
        list_append(self.queue, (unitype) -1, 'i'); // previous node ID for shortest path
    }
    // printf("setup!\n");
    *selfp = self;
}

void stepDijkstra(Dijkstra *selfp) {
    Dijkstra self = *selfp;
    if (self.finishedDijkstra) {
        self.finishedDijkstra = 2;
    }
    if (self.queue -> length > 0 && !self.finishedDijkstra) {
        self.stepNum += 1;
        Node head = *((Node *) self.nodes -> data[self.queue -> data[0].i].p);
        for (int i = 0; i < head.neighbors -> length; i += 2) {
            char visited = 0;
            for (int j = 0; j < self.queue -> length; j += 3) { // check if in queue
                if (self.queue -> data[j].i == head.neighbors -> data[i].i) {
                    visited = j;
                    break;
                }
            }
            if (visited) { // is in queue
                if (self.queue -> data[1].d + head.neighbors -> data[i + 1].d < self.queue -> data[visited + 1].d) { // if new path is shorter than old path
                    self.queue -> data[visited + 1] = (unitype) (self.queue -> data[1].d + head.neighbors -> data[i + 1].d);
                    self.queue -> data[visited + 2] = self.queue -> data[0];
                    int j = visited + 1;
                    while (self.queue -> data[j].d < self.queue -> data[j - 3].d) { // swap adjacents
                        unitype temp1 = self.queue -> data[j - 1];
                        unitype temp2 = self.queue -> data[j];
                        unitype temp3 = self.queue -> data[j + 1];
                        self.queue -> data[j - 1] = self.queue -> data[j - 4];
                        self.queue -> data[j] = self.queue -> data[j - 3];
                        self.queue -> data[j + 1] = self.queue -> data[j - 2];
                        self.queue -> data[j - 4] = temp1;
                        self.queue -> data[j - 3] = temp2;
                        self.queue -> data[j - 2] = temp3;
                        j -= 3;
                    }
                }
            } else {
                for (int j = 0; j < self.completed -> length; j += 3) { // check if in completed
                    if (self.completed -> data[j].i == head.neighbors -> data[i].i) {
                        visited = 1;
                        break;
                    }
                }
                if (!visited) { // is not in queue or completed lists
                    list_append(self.queue, (unitype) head.neighbors -> data[i].i, 'i');
                    list_append(self.queue, (unitype) (self.queue -> data[1].d + head.neighbors -> data[i + 1].d), 'd');
                    list_append(self.queue, (unitype) self.queue -> data[0].i, 'i');
                    int j = self.queue -> length - 2;
                    while (self.queue -> data[j].d < self.queue -> data[j - 3].d) { // swap adjacents
                        unitype temp1 = self.queue -> data[j - 1];
                        unitype temp2 = self.queue -> data[j];
                        unitype temp3 = self.queue -> data[j + 1];
                        self.queue -> data[j - 1] = self.queue -> data[j - 4];
                        self.queue -> data[j] = self.queue -> data[j - 3];
                        self.queue -> data[j + 1] = self.queue -> data[j - 2];
                        self.queue -> data[j - 4] = temp1;
                        self.queue -> data[j - 3] = temp2;
                        self.queue -> data[j - 2] = temp3;
                        j -= 3;
                    }
                }
            }
        }
        list_append(self.completed, self.queue -> data[0], 'i');
        list_append(self.completed, self.queue -> data[1], 'd');
        list_append(self.completed, self.queue -> data[2], 'i');
        list_delete(self.queue, 0);
        list_delete(self.queue, 0);
        list_delete(self.queue, 0);
        // printf("completed: ");
        // list_print(self.completed);
        // printf("queue: ");
        // list_print(self.queue);
        if (self.queue -> length == 0 || self.completed -> data[self.completed -> length - 3].i == self.end) {
            if (self.completed -> data[self.completed -> length - 3].i != self.end) {
                //printf("no path found\n");
            }
            //printf("finished!\n");
            self.finishedDijkstra = 1;
        } else {
            //printf("stepped!\n");
        }
    }
    *selfp = self;
}

void renderGraph(Dijkstra *selfp) { // renders the nodes
    Dijkstra self = *selfp;
    for (int i = self.nodeCount - 1; i > -1; i--) {
        Node render = *((Node *) (self.nodes -> data[i].p)); // cleanest C code 2023
        if (self.selected == i) {
            turtleGoto((render.xpos + self.screenX) * self.globalsize, (render.ypos + self.screenY) * self.globalsize);
            turtlePenColor(120, 120, 120);
            turtlePenSize(render.size * 1.1 * self.globalsize);
            turtlePenDown();
            turtlePenUp();
        }
        if (self.start == i) {
            turtlePenColor(self.specColor[0], self.specColor[1], self.specColor[2]);
        } else {
            if (self.end == i) {
                turtlePenColor(self.specColor[3], self.specColor[4], self.specColor[5]);
            } else {
                turtlePenColor(render.color[0], render.color[1], render.color[2]);
                int j = 0;
                for (; j < self.queue -> length; j += 3) {
                    if (i == self.queue -> data[j].i) {
                        turtlePenColor(self.specColor[6], self.specColor[7], self.specColor[8]);
                        break;
                    }
                }
                if (j >= self.queue -> length) {
                    j = 0;
                    for (; j < self.completed -> length; j += 3) {
                        if (i == self.completed -> data[j].i) {
                            turtlePenColor(self.specColor[9], self.specColor[10], self.specColor[11]);
                            break;
                        }
                    }
                }
                
            }
        }
        turtlePenSize(render.size * self.globalsize);
        turtleGoto((render.xpos + self.screenX) * self.globalsize, (render.ypos + self.screenY) * self.globalsize);
        // printf("size: %lf\n", render.size);
        turtlePenDown();
        turtlePenUp();
        turtlePenColor(30, 30, 30);
        textGLWriteString(render.text, (render.xpos + self.screenX) * self.globalsize, (render.ypos + self.screenY) * self.globalsize, render.size * 0.6 * self.globalsize, 50);
    }
    // *selfp = self; // no need to restoring
}

void renderShortestPath(Dijkstra *selfp) {
    Dijkstra self = *selfp;
    int i = self.completed -> length - 3;
    for (; i > -1; i -= 3) { // find end (should be the last one in the completed list but shit happens)
        if (self.completed -> data[i].i == self.end) {
            break;
        }
    }
    if (i < 0) {
        //printf("no path exists\n");
        return; // no path exists
    }
    turtleGoto((((Node *) self.nodes -> data[self.end].p) -> xpos + self.screenX) * self.globalsize, (((Node *) self.nodes -> data[self.end].p) -> ypos + self.screenY) * self.globalsize); 
    turtlePenColor(self.specColor[12], self.specColor[13], self.specColor[14]);
    turtlePenSize(4 * self.globalsize);
    turtlePenDown();
    int next = self.completed -> data[i + 2].i;
    while (next != -1) {
        turtleGoto((((Node *) self.nodes -> data[next].p) -> xpos + self.screenX) * self.globalsize, (((Node *) self.nodes -> data[next].p) -> ypos + self.screenY) * self.globalsize);
        for (int j = 0; j < self.completed -> length; j += 3) {
            if (self.completed -> data[j].i == next) { // should always find a match
                next = self.completed -> data[j + 2].i;
                //printf("next = %d\n", next);
                break;
            }
        }
    }
    turtlePenUp();
    *selfp = self; // just for safety
}

void renderConnections(Dijkstra *selfp) { // renders the connections between nodes
    Dijkstra self = *selfp;
    turtlePenSize(3 * self.globalsize);
    for (int i = 0; i < self.connections -> length; i += 3) {
        turtlePenColor(60, 60, 60);
        int j = 0;
        for (; j < self.queue -> length; j += 3) {
            if ((self.connections -> data[i].i == self.queue -> data[j].i && self.connections -> data[i + 1].i == self.queue -> data[j + 2].i) || (self.connections -> data[i + 1].i == self.queue -> data[j].i && self.connections -> data[i].i == self.queue -> data[j + 2].i)) {
                turtlePenColor(self.specColor[6], self.specColor[7], self.specColor[8]);
                break;
            }
        }
        if (j >= self.queue -> length) {
            j = 0;
            for (; j < self.completed -> length; j += 3) {
                if ((self.connections -> data[i].i == self.completed -> data[j].i && self.connections -> data[i + 1].i == self.completed -> data[j + 2].i) || (self.connections -> data[i + 1].i == self.completed -> data[j].i && self.connections -> data[i].i == self.completed -> data[j + 2].i)) {
                    turtlePenColor(self.specColor[9], self.specColor[10], self.specColor[11]);
                    break;
                }
            }
        }
        Node node1 = *((Node *) self.nodes -> data[self.connections -> data[i].i].p);
        Node node2 = *((Node *) self.nodes -> data[self.connections -> data[i + 1].i].p);
        self.connections -> data[i + 2] = (unitype) sqrt((node1.xpos - node2.xpos) * (node1.xpos - node2.xpos) + (node1.ypos - node2.ypos) * (node1.ypos - node2.ypos));
        /* update neighbors distance */
        int ind = -1;
        for (int j = 0; j < node1.neighbors -> length; j += 2) {
            if (node1.neighbors -> data[j].i == self.connections -> data[i + 1].i) {
                ind = j + 1;
                break;
            }
        }
        if (ind > -1)
            ((Node *) self.nodes -> data[self.connections -> data[i].i].p) -> neighbors -> data[ind] = self.connections -> data[i + 2];
        ind = -1;
        for (int j = 0; j < node2.neighbors -> length; j += 2) {
            if (node2.neighbors -> data[j].i == self.connections -> data[i].i) {
                ind = j + 1;
                break;
            }
        }
        if (ind > -1)
            ((Node *) self.nodes -> data[self.connections -> data[i + 1].i].p) -> neighbors -> data[ind] = self.connections -> data[i + 2];
        //printf("node: %d\n", self.connections -> data[i + 1].i);
        //list_print(((Node *) self.nodes -> data[self.connections -> data[i + 1].i].p) -> neighbors);
        turtleGoto((node1.xpos + self.screenX) * self.globalsize, (node1.ypos + self.screenY) * self.globalsize);
        turtlePenDown();
        turtleGoto((node2.xpos + self.screenX) * self.globalsize, (node2.ypos + self.screenY) * self.globalsize);
        turtlePenUp();
    }
    if (self.finishedDijkstra == 2) {
        renderShortestPath(&self);
    }
    if (self.selectMode == 4) {
        turtlePenColor(60, 60, 60);
        turtleGoto((((Node *) self.nodes -> data[self.wireStart].p) -> xpos + self.screenX) * self.globalsize, (((Node *) self.nodes -> data[self.wireStart].p) -> ypos + self.screenY) * self.globalsize);
        turtlePenDown();
        if (self.wireEnd == -1) {
            turtleGoto(self.mouseX, self.mouseY);
        } else {
            turtleGoto((((Node *) self.nodes -> data[self.wireEnd].p) -> xpos + self.screenX) * self.globalsize, (((Node *) self.nodes -> data[self.wireEnd].p) -> ypos + self.screenY) * self.globalsize);
        }
        turtlePenUp();
    }
    *selfp = self;
}

void renderConnectionLabels(Dijkstra *selfp) { // renders numberic labels for the connections
    Dijkstra self = *selfp;
    //turtlePenSize(3 * self.globalsize);
    turtlePenColor(60, 60, 60);
    for (int i = 0; i < self.connections -> length; i += 3) {
        Node node1 = *((Node *) self.nodes -> data[self.connections -> data[i].i].p);
        Node node2 = *((Node *) self.nodes -> data[self.connections -> data[i + 1].i].p);
        double writeX = ((node1.xpos + self.screenX) * self.globalsize + (node2.xpos + self.screenX) * self.globalsize) / 2;
        double writeY = ((node1.ypos + self.screenY) * self.globalsize + (node2.ypos + self.screenY) * self.globalsize) / 2;
        char num[10];
        sprintf(num, "%.0lf", self.connections -> data[i + 2].d);
        turtlePenColor(0, 0, 0);
        textGLWriteString(num, writeX, writeY, 10 * self.globalsize, 50);
    }
    *selfp = self; // just for safety
}
 
void mouseTick(Dijkstra *selfp) {
    Dijkstra self = *selfp;
    turtleGetMouseCoords(); // get the mouse coordinates (turtle.mouseX, turtle.mouseY)
    self.mouseX = turtle.mouseX;
    self.mouseY = turtle.mouseY;
    if (self.selectMode == 0 || self.selectMode == 4) {
        self.selected = -1;
        self.wireEnd = -1;
        for (int i = 0; i < self.nodes -> length; i++) {
            Node test = *((Node *) (self.nodes -> data[i].p));
            if ((self.mouseX - (test.xpos + self.screenX) * self.globalsize) * (self.mouseX - (test.xpos + self.screenX) * self.globalsize) + (self.mouseY - (test.ypos + self.screenY) * self.globalsize) * (self.mouseY - (test.ypos + self.screenY) * self.globalsize) < test.size * test.size * 0.25 * self.globalsize * self.globalsize) {
                if (self.selectMode == 0) {
                    self.selected = i;
                } else {
                    self.wireEnd = i;
                }
                break;
            }
        }
    }
    if (self.stepNum > 0) {
        self.selected = -1;
        self.selectMode = 0;
    }
    if (turtleMouseDown()) {
        if (self.keys[0] == 0) {
            self.keys[0] = 1;
            if (self.selected == -1) {
                self.selectMode = 0;
            } else {
                if (self.selectMode == 2) {
                    // printf("placed\n");
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
                    self.focalCSX = ((Node *) (self.nodes -> data[self.selected].p)) -> xpos;
                    self.focalCSY = ((Node *) (self.nodes -> data[self.selected].p)) -> ypos;
                    self.focalX = self.mouseX - (self.focalCSX + self.screenX) * self.globalsize;
                    self.focalY = self.mouseY - (self.focalCSY + self.screenY) * self.globalsize;
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
                ((Node *) (self.nodes -> data[self.selected].p)) -> xpos = (self.mouseX - self.focalX) / self.globalsize - self.screenX;
                ((Node *) (self.nodes -> data[self.selected].p)) -> ypos = (self.mouseY - self.focalY) / self.globalsize - self.screenY;
            } else {
                if (self.selectMode == 0) {
                    self.screenX = (self.mouseX - self.focalX) / self.globalsize + self.focalCSX;
                    self.screenY = (self.mouseY - self.focalY) / self.globalsize + self.focalCSY;
                }
            }
        }
    } else {
        if (self.keys[0] == 1) {
            if (self.selectMode == 4 && self.wireStart != -1 && self.wireEnd != -1) {
                Node wireStartNode = *((Node *) (self.nodes -> data[self.wireStart].p));
                char found = 0;
                for (int i = 0; i < wireStartNode.neighbors -> length; i += 2) {
                    if (wireStartNode.neighbors -> data[i].i == self.wireEnd) {
                        list_delete(wireStartNode.neighbors, i);
                        list_delete(wireStartNode.neighbors, i);
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    //printf("delete\n");
                    int temp = list_find(((Node *) (self.nodes -> data[self.wireEnd].p)) -> neighbors, (unitype) self.wireStart, 'i');
                    list_delete(((Node *) (self.nodes -> data[self.wireEnd].p)) -> neighbors, temp);
                    list_delete(((Node *) (self.nodes -> data[self.wireEnd].p)) -> neighbors, temp);
                    for (int i = 0; i < self.connections -> length; i += 3) {
                        if ((self.connections -> data[i].i == self.wireStart && self.connections -> data[i + 1].i == self.wireEnd) || (self.connections -> data[i].i == self.wireEnd && self.connections -> data[i + 1].i == self.wireStart)) {
                            list_delete(self.connections, i);
                            list_delete(self.connections, i);
                            list_delete(self.connections, i);
                            break;
                        }
                    }
                } else {
                    list_append(self.connections, (unitype) self.wireStart, 'i'); // add a new connection
                    list_append(self.connections, (unitype) self.wireEnd, 'i');
                    list_append(self.connections, (unitype) 0.0, 'd');
                    list_append(((Node *) self.nodes -> data[self.wireStart].p) -> neighbors, (unitype) self.wireEnd, 'i');
                    list_append(((Node *) self.nodes -> data[self.wireStart].p) -> neighbors, (unitype) 0.0, 'd');
                    list_append(((Node *) self.nodes -> data[self.wireEnd].p) -> neighbors, (unitype) self.wireStart, 'i');
                    list_append(((Node *) self.nodes -> data[self.wireEnd].p) -> neighbors, (unitype) 0.0, 'd');
                }
            }
            //self.selected = -1;
            if (self.selected != -1 && fabs(((Node *) (self.nodes -> data[self.selected].p)) -> xpos - self.focalCSX) < 0.01 && fabs(((Node *) (self.nodes -> data[self.selected].p)) -> ypos - self.focalCSY) < 0.01) {
                //printf("clicked on %d\n", self.selected);
                if (self.toggle == 0) {
                    self.start = self.selected;
                    self.toggle = 1;
                } else {
                    if (self.selected != self.start) {
                        self.end = self.selected;
                        self.toggle = 0;
                        setupDijkstra(&self);
                    }
                }
                //printf("start: %d, end %d\n", self.start, self.end);
            }
            if (self.selected == -1 && fabs(self.focalCSX - self.screenX) < 0.01 && fabs(self.focalCSY - self.screenY) && self.stepNum == 0) {
                self.toggle = 0;
                self.start = -1;
                self.end = -1;
                list_clear(self.queue);
                list_clear(self.completed);
            }
            self.selectMode = 0;
            self.keys[0] = 0;
        }
        if (self.selectMode == 2) {
            ((Node *) (self.nodes -> data[self.selected].p)) -> xpos = (self.mouseX - self.focalX) / self.globalsize - self.screenX;
            ((Node *) (self.nodes -> data[self.selected].p)) -> ypos = (self.mouseY - self.focalY) / self.globalsize - self.screenY;
        }
    }
    *selfp = self;
}

void scrollTick(Dijkstra *selfp) {
    Dijkstra self = *selfp;
    double mouseWheel = turtleMouseWheel(); // behavior is a bit different for the scroll wheel
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

void hotkeyTick(Dijkstra *selfp) {
    Dijkstra self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE)) {
        if (self.keys[1] == 0) {
            self.keys[1] = 1;
            stepDijkstra(&self);
        }
    } else {
        self.keys[1] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_E)) {
        if (self.keys[2] == 0) {
            self.keys[2] = 1;
            if (self.showDistances) {
                self.showDistances = 0;
            } else {
                self.showDistances = 1;
            }
        }
    } else {
        self.keys[2] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_C)) {
        if (self.keys[3] == 0 && self.stepNum == 0) {
            self.keys[3] = 1;
            self.start = -1;
            self.end = -1;
            self.toggle = 0;
            self.finishedDijkstra = 0;
            self.selected = -1;
            self.selectMode = 0;
            self.nodeCount = 0;
            for (int i = 0; i < self.nodes -> length; i++) {
                list_free(((Node *) self.nodes -> data[i].p) -> neighbors);
                free(((Node *) self.nodes -> data[i].p) -> text);
            }
            list_clear(self.nodes);
            list_clear(self.connections);
            list_clear(self.queue);
            list_clear(self.completed);
        }
    } else {
        self.keys[3] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_R)) {
        if (self.keys[4] == 0) {
            self.keys[4] = 1;
            self.start = -1;
            self.end = -1;
            self.toggle = 0;
            self.finishedDijkstra = 0;
            self.selected = -1;
            self.selectMode = 0;
            self.stepNum = 0;
            list_clear(self.queue);
            list_clear(self.completed);
        }
    } else {
        self.keys[4] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_1)) {
        if (self.keys[5] == 0) {
            self.keys[5] = 1;
            char num[12];
            sprintf(num, "%d", self.nodes -> length);
            self.selected = self.nodes -> length;
            self.selectMode = 2;
            list_append(self.nodes, (unitype) (void *) newNode(self.nodes -> length, (self.mouseX / self.globalsize - self.screenX), (self.mouseY / self.globalsize - self.screenY), randomDouble(20, 50), 100, 100, 100, num), 'p');
            self.focalCSX = ((Node *) (self.nodes -> data[self.selected].p)) -> xpos;
            self.focalCSY = ((Node *) (self.nodes -> data[self.selected].p)) -> ypos;
            self.focalX = self.mouseX - (self.focalCSX + self.screenX) * self.globalsize;
            self.focalY = self.mouseY - (self.focalCSY + self.screenY) * self.globalsize;
            self.nodeCount++;
        }
    } else {
        self.keys[5] = 0;
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
    window = glfwCreateWindow(1280, 720, "Dijkstra's Algorithm", NULL, NULL);
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
    Dijkstra obj; // principle object
    srand(time(NULL)); // randomiser init seed
    if (argc == 1) {
        init(&obj, 20);
    } else {
        int nodeCount = 0;
        sscanf(argv[1], "%d\n", &nodeCount);
        init(&obj, nodeCount);
    }
    // for (int i = 0; i < 20; i++) {
    //     printf("%lf\n", ((Node *) obj.nodes -> data[i].p) -> size);
    // }
    
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