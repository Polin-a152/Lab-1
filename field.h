#ifndef FIELD_H
#define FIELD_H

#define MAX 100

// Символы объектов
#define EMPTY '_'
#define DINO  '#'
#define PIT   '%'
#define TREE  '&'
#define STONE '@'
#define MNT   '^'

// Глобальное поле
extern char field[MAX][MAX];

// Размеры
extern int width;
extern int height;

// Позиция динозавра
extern int dinoX;
extern int dinoY;

// Флаг старта
extern int hasStart;

void initFieldGlobals();

#endif
