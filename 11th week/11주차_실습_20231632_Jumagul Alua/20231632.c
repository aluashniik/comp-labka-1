#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WALL 1
#define SPACE 0

typedef struct {
    int parent; 
    int rank;
} Set;

void drawHori(FILE *fp, int width, const int *walls) {
    for (int i = 0; i < width; i++) {
        fprintf(fp, "+");
        if (walls[i] == WALL) {
            fprintf(fp, "-");
        } else {
            fprintf(fp, " ");
        }
    }
    fprintf(fp, "+\n");
}

void drawVert(FILE *fp, int width, const int *walls) {
    fprintf(fp, "|");
    for (int i = 0; i < width - 1; i++) {
        fprintf(fp, " ");
        if (walls[i] == WALL) {
            fprintf(fp, "|");
        } else {
            fprintf(fp, " ");
        }
    }
    fprintf(fp, " |\n");
}

void initSet(Set *set, int size) {
    for (int i = 0; i < size; i++) {
        set[i].parent = i;
        set[i].rank = 0;
    }
}

int find(Set *set, int x) {
    if (set[x].parent != x) {
        set[x].parent = find(set, set[x].parent);
    }
    return set[x].parent;
}

void uSets(Set *set, int x, int y) {
    int rootX = find(set, x);
    int rootY = find(set, y);

    if (rootX == rootY) {
        return;
    }
    if (set[rootX].rank < set[rootY].rank) {
        set[rootX].parent = rootY;
    } else if (set[rootX].rank > set[rootY].rank) {
        set[rootY].parent = rootX;
    } else {
        set[rootY].parent = rootX;
        set[rootX].rank++;
    }
}

typedef struct {
    int x1, y1, x2, y2;
} Wall;

void shuffle(Wall *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Wall temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void generate(FILE *fp, int width, int height) {
    srand(time(NULL));

    Set *sets = (Set *)malloc(width * height * sizeof(Set));
    initSet(sets, width * height);

    int hWallsSize = width * (height-1);
    int vWallsSize = (width-1) * height;
    int totalWalls = hWallsSize + vWallsSize;

    Wall *walls = (Wall *)malloc(totalWalls * sizeof(Wall));
    int index = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x < width - 1) {
                walls[index++] = (Wall){x, y, x + 1, y};
            }
            if (y < height - 1) {
                walls[index++] = (Wall){x, y, x, y + 1};
            }
        }
    }

    shuffle(walls, totalWalls);

    int *hWalls = (int*)malloc(hWallsSize * sizeof(int));
    int *vWalls = (int*)malloc(vWallsSize * sizeof(int));

    for (int i = 0; i < hWallsSize; i++) {
        hWalls[i] = WALL;
    }

    for (int i = 0; i < vWallsSize; i++) {
        vWalls[i] = WALL;
    }

    for (int i = 0; i < totalWalls; i++) {
        Wall wall = walls[i];
        int cell1 = wall.y1 * width + wall.x1;
        int cell2 = wall.y2 * width + wall.x2;

        if (find(sets, cell1) != find(sets, cell2)) {
            uSets(sets, cell1, cell2);

            if (wall.x1 == wall.x2) {
                hWalls[wall.y1 * width + wall.x1] = SPACE;
            } else {
                vWalls[wall.y1 * (width - 1) + wall.x1] = SPACE;
            }
        }
    }

    // 첫 번째 줄 그리기
    for (int i = 0; i < width; i++) {
        fprintf(fp, "+-");
    }
    fprintf(fp, "+\n");

    // 나머지 미로 그리기
    for (int y = 0; y < height - 1; y++) {
        drawVert(fp, width, &vWalls[y * (width - 1)]);
        drawHori(fp, width, &hWalls[y * width]);
    }

    // 마지막 줄 인접한 셀의 모든 벽 제거
    for (int x = 0; x < width - 1; x++) {
        int cell1 = (height - 1) * width + x;
        int cell2 = (height - 1) * width + x + 1;
        if (find(sets, cell1) != find(sets, cell2)) {
            vWalls[(height - 1) * (width - 1) + x] = SPACE;
            uSets(sets, cell1, cell2);
        }
    }

    drawVert(fp, width, &vWalls[(height - 2) * (width - 1)]);

    // 마지막 줄 그리기
    for (int i = 0; i < width; i++) {
        fprintf(fp, "+-");
    }
    fprintf(fp, "+\n");

    free(sets);
    free(hWalls);
    free(vWalls);
    free(walls);
}

int main() {
    FILE *fp = fopen("output.maz", "wt");
    int width, height;
    scanf("%d %d", &width, &height);
    generate(fp, width, height);
    fclose(fp);
    return 0;
}



