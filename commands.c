#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "field.h"
#include "undo.h"

// Создаёт новое поле
void cmd_SIZE(int w, int h) {
    if (w <= 0 || h <= 0 || w > MAX || h > MAX) {
        printf("Ошибка: неверные размеры.\n");
        exit(1);
    }
    //нет ошиб
    undo_push(); 
    width = w;
    height = h;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            field[y][x] = EMPTY;
}

// Ставит динозавра в указанную позицию
void cmd_START(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        printf("Ошибка: динозавр вне поля.\n");
        exit(1);
    }
    // нет ошиб
    undo_push(); 
    field[y][x] = DINO;
    dinoX = x; dinoY = y;
    hasStart = 1;
}

// Двигает динозавра на одну клетку
void cmd_MOVE(char *dir) {
    undo_push(); 
    int dx = 0, dy = 0;
    if (!strcmp(dir, "UP")) dy = -1;
    else if (!strcmp(dir, "DOWN")) dy = 1;
    else if (!strcmp(dir, "LEFT")) dx = -1;
    else if (!strcmp(dir, "RIGHT")) dx = 1;
    else { printf("Ошибка: неверное направление.\n"); return; }

    int newX = (dinoX + dx + width) % width;
    int newY = (dinoY + dy + height) % height;
    if (field[newY][newX] == PIT) {
        printf("Ошибка: дино упал в яму!\n");
        exit(1);
    }
    //Живой, обход 
    if (field[newY][newX] == MNT || field[newY][newX] == TREE || field[newY][newX] == STONE) {
        printf("Предупреждение: препятствие впереди.\n");
        return;
    }
    field[dinoY][dinoX] = EMPTY;
    dinoX = newX; dinoY = newY;
    field[dinoY][dinoX] = DINO;
}

// Красит клетку, где стоит динозавр
void cmd_PAINT(char color) {
    undo_push(); 
    field[dinoY][dinoX] = color;
    printf("Клетка окрашена в '%c'\n", color);
}

// Делает яму в выбранном направлении
void cmd_DIG(char *dir) {
    undo_push(); 
    int nx = dinoX, ny = dinoY;

    // Определяем направление
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    else {
        printf("Ошибка: неверное направление в DIG.\n");
        return;
    }

    // Обход границ (телепорт через край)
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
    if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;

    // Проверки, что в этой клетке можно копать
    if (field[ny][nx] == DINO) {
        printf("Ошибка: нельзя копать яму под динозавром!\n");
        return;
    }

    if (field[ny][nx] == PIT) {
        printf("Ошибка: здесь уже есть яма.\n");
        return;
    }

    if (field[ny][nx] == MNT) {
        printf("Ошибка: нельзя копать яму в горе.\n");
        return;
    }

    if (field[ny][nx] == TREE) {
        printf("Ошибка: нельзя копать яму в дереве.\n");
        return;
    }

    if (field[ny][nx] == STONE) {
        printf("Ошибка: нельзя копать яму в камне.\n");
        return;
    }

    // создаём яму
    field[ny][nx] = PIT;
    printf("Яма создана в направлении %s.\n", dir);
}

// Делает гору в направлении, может засыпать яму
void cmd_MOUND(char *dir) {
    undo_push(); 
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
    undo_push(); 
    int dx = 0, dy = 0;
    if (!strcmp(dir, "UP")) dy = -1;
    else if (!strcmp(dir, "DOWN")) dy = 1;
    else if (!strcmp(dir, "LEFT")) dx = -1;
    else if (!strcmp(dir, "RIGHT")) dx = 1;

    int x = dinoX, y = dinoY;
    for (int i = 0; i < n; i++) {
        x = (x + dx + width) % width;
        y = (y + dy + height) % height;
        if (field[y][x] == MNT || field[y][x] == TREE || field[y][x] == STONE) {
            printf("Предупреждение: препятствие во время прыжка.\n");
            return;
        }
    }
    if (field[y][x] == PIT) {
        printf("Ошибка: приземление в яму.\n");
        exit(1);
    }
    field[dinoY][dinoX] = EMPTY;
    dinoX = x; dinoY = y;
    field[dinoY][dinoX] = DINO;
}

// Создаёт дерево (&)
void cmd_GROW(char *dir) {
    undo_push(); 
    int nx = dinoX, ny = dinoY;
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
    if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;
    if (field[ny][nx] == EMPTY)
        field[ny][nx] = TREE;
    else
        printf("Ошибка: дерево можно посадить только в пустой клетке.\n");
}

// Срубает дерево (&)
void cmd_CUT(char *dir) {
    undo_push(); 
    int nx = dinoX, ny = dinoY;
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;
    if (field[ny][nx] == TREE)
        field[ny][nx] = EMPTY;
    else
        printf("Ошибка: рядом нет дерева для срубания.\n");
}

// Создаёт камень (@)
void cmd_MAKE(char *dir) {
    undo_push(); 
    int nx = dinoX, ny = dinoY;
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
    if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;
    if (field[ny][nx] == EMPTY)
        field[ny][nx] = STONE;
    else
        printf("Ошибка: нельзя создать камень не на пустой клетке.\n");
}

// Пинает камень (@)
void cmd_PUSH(char *dir) {
    undo_push(); 
    int nx = dinoX, ny = dinoY;
    if (!strcmp(dir, "UP")) ny--;
    else if (!strcmp(dir, "DOWN")) ny++;
    else if (!strcmp(dir, "LEFT")) nx--;
    else if (!strcmp(dir, "RIGHT")) nx++;
    if (nx < 0) nx = width - 1;
    if (ny < 0) ny = height - 1;
    if (nx >= width) nx = 0;
    if (ny >= height) ny = 0;

    if (field[ny][nx] != STONE) {
        printf("Ошибка: рядом нет камня для пинка.\n");
        return;
    }

    int tx = (nx + (nx - dinoX) + width) % width;
    int ty = (ny + (ny - dinoY) + height) % height;

    if (field[ty][tx] == TREE || field[ty][tx] == MNT) {
        printf("Предупреждение: камень не может пройти через препятствие.\n");
        return;
    }

    if (field[ty][tx] == PIT) {
        field[ty][tx] = EMPTY;
        field[ny][nx] = EMPTY;
        printf("Камень засыпал яму.\n");
        return;
    }

    if (field[ty][tx] == EMPTY) {
        field[ty][tx] = STONE;
        field[ny][nx] = EMPTY;
    }
}

// Разбирает и вызывает нужную команду
int exec_command_line(const char *line, int lineno) {

    char cmd[32];

    if (sscanf(line, "%31s", cmd) != 1)
        return 0;

    if (!strcmp(cmd, "SIZE")) {
        int w, h, end;
        char extra;

        int count = sscanf(line, "%*s %d %d %n%c", &w, &h, &end, &extra);
        if (count != 2 || line[end] != '\0') {
            printf("Ошибка: SIZE требует два числа.\n");
            return -1;
        }

        cmd_SIZE(w, h);
    }

    else if (!strcmp(cmd, "START")) {
        int x, y, end;
        char extra;

        int count = sscanf(line, "%*s %d %d %n%c", &x, &y, &end, &extra);
        if (count != 2 || line[end] != '\0') {
            printf("Ошибка: START требует два числа.\n");
            return -1;
        }

        cmd_START(x, y);
    }

    else if (!strcmp(cmd, "MOVE")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: MOVE требует одно направление.\n");
            return -1;
        }

        cmd_MOVE(dir);
    }

    else if (!strcmp(cmd, "PAINT")) {
        char color, extra;
        int end;

        int count = sscanf(line, "%*s %c %n%c", &color, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: PAINT требует один символ.\n");
            return -1;
        }

        cmd_PAINT(color);
    }

    else if (!strcmp(cmd, "DIG")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: DIG требует одно направление.\n");
            return -1;
        }

        cmd_DIG(dir);
    }

    else if (!strcmp(cmd, "MOUND")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: MOUND требует одно направление.\n");
            return -1;
        }

        cmd_MOUND(dir);
    }

    else if (!strcmp(cmd, "GROW")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: GROW требует одно направление.\n");
            return -1;
        }

        cmd_GROW(dir);
    }

    else if (!strcmp(cmd, "CUT")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: CUT требует одно направление.\n");
            return -1;
        }

        cmd_CUT(dir);
    }

    else if (!strcmp(cmd, "MAKE")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: MAKE требует одно направление.\n");
            return -1;
        }

        cmd_MAKE(dir);
    }

    else if (!strcmp(cmd, "PUSH")) {
        char dir[20], extra;
        int end;

        int count = sscanf(line, "%*s %19s %n%c", dir, &end, &extra);
        if (count != 1 || line[end] != '\0') {
            printf("Ошибка: PUSH требует одно направление.\n");
            return -1;
        }

        cmd_PUSH(dir);
    }

    else if (!strcmp(cmd, "JUMP")) {
        char dir[20], extra;
        int n, end;

        int count = sscanf(line, "%*s %19s %d %n%c", dir, &n, &end, &extra);
        if (count != 2 || line[end] != '\0') {
            printf("Ошибка: JUMP требует направление и число.\n");
            return -1;
        }

        cmd_JUMP(dir, n);
    }else if (!strcmp(cmd, "UNDO")) {
        if (undo_pop() != 0) {
            printf("Ошибка: нет состояний для отката.\n");
        }

    }else {
        printf("Ошибка: неизвестная команда '%s'.\n", cmd);
        return -1;
    }

    return 0;
}









