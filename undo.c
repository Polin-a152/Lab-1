#include <string.h>
#include "field.h"
#include "undo.h"

#define MAX_UNDO 50

static char history[MAX_UNDO][MAX][MAX];
static int wHist[MAX_UNDO], hHist[MAX_UNDO];
static int xHist[MAX_UNDO], yHist[MAX_UNDO];
static int top = -1;

// Сбрасывает стек откатов
void undo_init() { top = -1; }

// Сохраняет текущее состояние
void undo_push() {
    if (top < MAX_UNDO - 1) top++;
    wHist[top] = width;
    hHist[top] = height;
    xHist[top] = dinoX;
    yHist[top] = dinoY;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            history[top][y][x] = field[y][x];
}

// Восстанавливает предыдущее состояние
int undo_pop() {
    if (top < 0) return -1;
    width = wHist[top];
    height = hHist[top];
    dinoX = xHist[top];
    dinoY = yHist[top];
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            field[y][x] = history[top][y][x];
    top--;
    return 0;
}
