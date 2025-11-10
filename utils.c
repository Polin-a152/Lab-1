#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"

// Очищает экран
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Держит паузу в секундах
void sleepSeconds(int sec) {
#ifdef _WIN32
    Sleep(sec * 1000);
#else
    struct timespec t = {sec, 0};
    nanosleep(&t, NULL);
#endif
}

// Создаёт папку вывода, если её нет
int makeOutputDir(const char *name) {
#ifdef _WIN32
    _mkdir(name);
#else
    mkdir(name, 0777);
#endif
    return 0;
}
