#include <stdio.h>
#include <stdlib.h>
#include "field.h"

// Инициализирует все глобальные переменные
void initFieldGlobals() {
    width = height = 0;
    dinoX = dinoY = 0;
    hasStart = 0;
}

// Выводит текущее состояние поля
void printField() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++)
            printf("%c ", field[y][x]);
        printf("\n");
    }
}

// Сохраняет поле в текстовый файл
int saveFieldToFile(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++)
            fputc(field[y][x], f);
        fputc('\n', f);
    }
    fclose(f);
    return 0;
}

// Загружает поле из файла
int loadFieldFromFile(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    int y = 0;
    while (!feof(f) && y < MAX) {
        for (int x = 0; x < MAX; x++) {
            int c = fgetc(f);
            if (c == EOF || c == '\n') break;
            field[y][x] = (char)c;
        }
        y++;
    }
    fclose(f);
    height = y;
    return 0;
}
