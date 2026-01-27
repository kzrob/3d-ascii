#include <stdio.h>
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
    v.x = v.x*cos(angle) - v.z*sin(angle);
    v.y = v.y;
    v.z = v.x*sin(angle) + v.z*cos(angle);
    return v;
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

void drawCanvas() {
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

void putPoint(Point p) {
    if (p.x < 0 || p.y < 0 || p.x >= SIZE || p.y >= SIZE) {return;}
    char c = getASCII(p.opacity);
    canvas[p.x][p.y] = c;
}

void putRect(int x, int y, int xs, int ys, float opacity) {
    for (int i=x; i<x+xs; i++) {
        for (int j=y; j<y+ys; j++) {
            Point p = {i, j, opacity};
            putPoint(p);
        }
    }
}

int main() {
    float t = 0;
    while (1) {
        clearCanvas();
        int VerticesSize = sizeof(Vertices)/sizeof(Vertices[0]);
        for (int i=0; i<VerticesSize; i++) {
            Vec3 v = Vertices[i];
            v = rotateXZ(v, t);
            Point p = project(v);
            putPoint(p);
        }
        drawCanvas();

        t += 1/FPS;
        usleep((int)(1000000.0/FPS)); // usleep takes microseconds
    }

    return 0;
}
