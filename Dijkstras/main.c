#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "priority_queue.h"

typedef struct {
    Vector2 pos;
    Vector2 parent;
    int cost;
    int totalCost;
    bool visited;
} GridNode;

typedef struct {
    GridNode** data;
    int rowCount;
    int columnCount;
} Grid;

enum GridDirection {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    ITERATION_END
};

Grid GridCreate(int rowCount, int columnCount) {
    Grid grid = {
        .rowCount = rowCount,
        .columnCount = columnCount,
        .data = malloc(sizeof(GridNode*)*rowCount)
    };

    for (int y = 0; y < rowCount; y++) {
        grid.data[y] = malloc(sizeof(GridNode)*columnCount);

        for (int x = 0; x < columnCount; x++) {
            grid.data[y][x] = (GridNode) {
                .pos = (Vector2) {x, y},
                .cost = (rand() % 9) + 1, 
                .totalCost = INT_MAX,
                .visited = false,
                .parent = INVALID_VEC2
            };
        }
    }

    return grid;
}

GridNode* GridGetNodeAtDirection(Grid* grid, Vector2 currentPos, enum GridDirection direction) {
    switch (direction) {
        case NORTH: {
            if (currentPos.y-1 > -1) {
                return &grid->data[currentPos.y-1][currentPos.x];
            }
        } break;
        case SOUTH: {
            if (currentPos.y+1 < grid->rowCount) {
                return &grid->data[currentPos.y+1][currentPos.x];
            }
        } break;
        case EAST: {
            if (currentPos.x-1 < -1) {
                return &grid->data[currentPos.y][currentPos.x-1];
            }
        } break;
        case WEST: {
            if (currentPos.x+1 < grid->columnCount) {
                return &grid->data[currentPos.y][currentPos.x+1];
            }
        } break;
    };

    return NULL;
}

bool Vector2InPath(Vector2* path, int count, Vector2 element) {
    for (int i = 0; i < count; i++) {
        if (path[i].x == element.x && path[i].y == element.y) {
            return true;
        }
    }

    return false;
}

int main(void) {
    srand(time(NULL));

    Grid grid = GridCreate(20, 40);

    PriorityQueue frontier = {};
    PriorityQueueInitialize(&frontier, grid.rowCount*grid.columnCount);

    Vector2 startCell = { 5, 2 };
    Vector2 endCell = { 10, 19 };

    grid.data[startCell.y][startCell.x].totalCost = 0;

    Enqueue(&frontier, startCell, 0);

    while (!PriorityQueueIsEmpty(frontier)) {
        Vec2AndCost currentNode = Dequeue(&frontier);
        Vector2 currentPos = currentNode.vec2;

        grid.data[currentPos.y][currentPos.x].visited = true;

        for (enum GridDirection direction = NORTH; direction < ITERATION_END; direction++) {
            GridNode* nodeInDirection = GridGetNodeAtDirection(&grid, currentPos, direction);

            if (nodeInDirection == NULL) {
                continue;
            }

            Vector2 vecInDirection = nodeInDirection->pos;
            int totalCostToMoveToNode = grid.data[currentPos.y][currentPos.x].totalCost + nodeInDirection->cost;

            if (totalCostToMoveToNode < nodeInDirection->totalCost) {
                nodeInDirection->totalCost = totalCostToMoveToNode;
                nodeInDirection->parent = currentPos;
            }

            if (!nodeInDirection->visited) {
                Enqueue(&frontier, vecInDirection, nodeInDirection->totalCost);
            }
        }
    }

    Vector2* path = calloc(grid.columnCount*grid.rowCount, sizeof(Vector2));
    int pathIndex = 0;

    for (GridNode currentNode = grid.data[endCell.y][endCell.x];
         currentNode.parent.y != -1 && currentNode.parent.x != -1;
         pathIndex++)
    {
        path[pathIndex] = currentNode.pos;
        currentNode = grid.data[currentNode.parent.y][currentNode.parent.x];
    }

    path[pathIndex] = startCell;
    pathIndex++;


    bool lineInPath = false;
    for (int y = 0; y < grid.rowCount; y++) {
        for (int x = 0; x < grid.columnCount; x++) {
            bool cellInPath = Vector2InPath(path, pathIndex, (Vector2){x, y});
            if (cellInPath) {
                printf("\x1B[32m%d\x1B[0m", grid.data[y][x].cost);
            }
            else {
                printf("%d", grid.data[y][x].cost);
            }

            if (x < grid.columnCount-1) {
                if (Vector2InPath(path, pathIndex, (Vector2){x+1, y}) && cellInPath) {
                    printf("\x1B[32m--\x1B[0m");
                }
                else {
                    printf("--");
                }
            }
        }

        printf("\n");

        if (y < grid.rowCount-1) {
            for (int x = 0; x < grid.columnCount; x++) {
                if (Vector2InPath(path, pathIndex, (Vector2){x, y}) &&
                    Vector2InPath(path, pathIndex, (Vector2){x, y+1}))
                {
                    printf("\x1B[32m|\x1B[0m  ");
                }
                else {
                    printf("|  ");
                }
            }

            printf("\n");
        }
    }

    return 0;
}
