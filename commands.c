#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "field.h"
#include "undo.h"

// Создаёт новое поле
void cmd_SIZE(int w, int h) {
    if (w <= 0  h <= 0  w > MAX || h > MAX) {
        printf("Ошибка: неверные размеры.\n");
        exit(1);
    }
    width = w;
    height = h;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            field[y][x] = EMPTY;
}

// Ставит динозавра в указанную позицию
void cmd_START(int x, int y) {
    if (x < 0  y < 0  x >= width || y >= height) {
        printf("Ошибка: динозавр вне поля.\n");
        exit(1);
    }
    field[y][x] = DINO;
    dinoX = x; dinoY = y;
    hasStart = 1;
}

// Двигает динозавра на одну клетку
void cmd_MOVE(char *dir) {
    int dx = 0, dy = 0;
    if (!strcmp(dir, "UP")) dy = -1;
    else if (!strcmp(dir, "DOWN")) dy = 1;
    else if (!strcmp(dir, "LEFT")) dx = -1;
    else if (!strcmp(dir, "RIGHT")) dx = 1;
    else { printf("Ошибка: неверное направление.\n"); return; }

    int newX = (dinoX + dx + width) % width;
    int newY = (dinoY + dy + height) % height;
    if (field[newY][newX] == PIT) {
        printf("Ошибка: динозавр упал в яму!\n");
        exit(1);
    }
    if (field[newY][newX] == MNT  field[newY][newX] == TREE  field[newY][newX] == STONE) {
        printf("Предупреждение: препятствие впереди.\n");
        return;
    }
    field[dinoY][dinoX] = EMPTY;
    dinoX = newX; dinoY = newY;
    field[dinoY][dinoX] = DINO;
}

// Красит клетку, где стоит динозавр
void cmd_PAINT(char color) {
    field[dinoY][dinoX] = color;
    printf("Клетка окрашена в '%c'\n", color);
}

// Делает яму в выбранном направлении
void cmd_DIG(char *dir) {
    int nx = dinoX, ny = dinoY;
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
    if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;
    field[ny][nx] = PIT;
}

// Делает гору в направлении, может засыпать яму
void cmd_MOUND(char *dir) {
    int nx = dinoX, ny = dinoY;
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
    if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;
    if (field[ny][nx] == PIT) field[ny][nx] = EMPTY;
    else field[ny][nx] = MNT;
}

// Прыжок динозавра
void cmd_JUMP(char *dir, int n) {
    int dx = 0, dy = 0;
    if (!strcmp(dir, "UP")) dy = -1;
    else if (!strcmp(dir, "DOWN")) dy = 1;
    else if (!strcmp(dir, "LEFT")) dx = -1;
    else if (!strcmp(dir, "RIGHT")) dx = 1;

    int x = dinoX, y = dinoY;
    for (int i = 0; i < n; i++) {
        x = (x + dx + width) % width;
        y = (y + dy + height) % height;
        if (field[y][x] == MNT) break;
    }
    if (field[y][x] == PIT) {
        printf("Ошибка: приземление в яму.\n");
        exit(1);
    }
    field[dinoY][dinoX] = EMPTY;
    dinoX = x; dinoY = y;
    field[dinoY][dinoX] = DINO;
}

// Разбирает и вызывает нужную команду
int exec_command_line(const char *line, int lineno) {
    char cmd[32];
    if (sscanf(line, "%31s", cmd) != 1) return 0;

    if (!strcmp(cmd, "SIZE")) {
        int w, h; sscanf(line, "%*s %d %d", &w, &h);
        cmd_SIZE(w, h);
    } else if (!strcmp(cmd, "START")) {
        int x, y; sscanf(line, "%*s %d %d", &x, &y);
        cmd_START(x, y);
    } else if (!strcmp(cmd, "MOVE")) {
        char dir[10]; sscanf(line, "%*s %s", dir);
        cmd_MOVE(dir);
    } else if (!strcmp(cmd, "PAINT")) {
        char c; sscanf(line, "%*s %c", &c);
        cmd_PAINT(c);
    } else if (!strcmp(cmd, "DIG")) {
        char dir[10]; sscanf(line, "%*s %s", dir);
        cmd_DIG(dir);
    } else if (!strcmp(cmd, "MOUND")) {
        char dir[10]; sscanf(line, "%*s %s", dir);
        cmd_MOUND(dir);
    } else if (!strcmp(cmd, "JUMP")) {
        char dir[10]; int n; sscanf(line, "%*s %s %d", dir, &n);
        cmd_JUMP(dir, n);
    } else {
        printf("Неизвестная команда в строке %d: %s\n", lineno, cmd);
        return -1;
    }
    return 0;
}
