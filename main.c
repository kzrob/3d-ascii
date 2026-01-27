#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define SIZE 100
float FPS = 24;

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;
typedef struct Vec2 {
    int x;
    int y;
} Vec2;
typedef struct Point {
    int x;
    int y;
    float opacity;
} Point;

Vec3 Camera = {0, 0, -3};
Vec3 Vertices[] = {
    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1},
    {-1,  1, -1},
    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    {-1,  1,  1},
};
Vec2 Edges[] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
};

char ASCII[] = " .:-=+*#%@";
char canvas[SIZE][SIZE];

Vec3 rotateXZ(Vec3 v, float angle) {
    Vec3 new = v;
    new.x = v.x*cos(angle) - v.z*sin(angle);
    new.z = v.x*sin(angle) + v.z*cos(angle);
    return new;
}

Point project(Vec3 v) {
    Point p;
    Vec3 d = {v.x - Camera.x, v.y - Camera.y, v.z - Camera.z};
    p.x = roundf((d.x/d.z + 1)/2 * SIZE/2);
    p.y = roundf((1 - d.y/d.z)/2 * SIZE/2);
    p.opacity = 1/d.z;
    return p;
}

// opacity ranges from 0 to 1
char getASCII(float opacity) {
    int size = strlen(ASCII)-1;
    int index = (int)(size*opacity);
    return ASCII[index];
}

void renderCanvas() {
    printf("\033[H"); // move cursor to top-left
    for (int i=0; i<SIZE; i++) {
        for (int j=0; j<SIZE; j++) {
            char c = canvas[i][j];
            printf("%c%c", c, c);
        }
        printf("\n");
    }
}

void clearCanvas() {
    memset(canvas, ' ', sizeof(canvas));
}

void drawPoint(Point p) {
    if (p.x < 0 || p.y < 0 || p.x >= SIZE || p.y >= SIZE) {return;}
    char c = getASCII(p.opacity);
    canvas[p.x][p.y] = c;
}

// Bresenham's line algorithm
void drawLine(int x1, int y1, int x2, int y2, float opacity) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        canvas[y1][x1] = getASCII(opacity);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void drawRect(int x, int y, int xs, int ys, float opacity) {
    for (int i=x; i<x+xs; i++) {
        for (int j=y; j<y+ys; j++) {
            Point p = {i, j, opacity};
            drawPoint(p);
        }
    }
}

int main() {
    float t = 0;
    while (1) {
        clearCanvas();
        int EdgesSize = sizeof(Edges)/sizeof(Edges[0]);
        for (int i=0; i<EdgesSize; i++) {
            Vec2 edge = Edges[i];
            Point p1 = project(rotateXZ(Vertices[edge.x], t));
            Point p2 = project(rotateXZ(Vertices[edge.y], t));
            float opacity = (p1.opacity + p2.opacity) / 2;
            drawLine(p1.x, p1.y, p2.x, p2.y, opacity);
        }
        renderCanvas();

        t += 1/FPS;
        usleep((int)(1000000.0/FPS)); // usleep takes microseconds
    }

    return 0;
}
