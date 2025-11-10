#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "field.h"
#include "commands.h"
#include "utils.h"
#include "undo.h"

static int opt_no_display = 0;
static int opt_no_save = 0;
static int opt_interval_sec = 1;

// Выполняет команды из файла
int process_file(const char *filename, int is_nested);

// Проверяет начало строки
static int starts_with(const char *s, const char *prefix) {
    size_t n = strlen(prefix);
    return strncmp(s, prefix, n) == 0;
}

// Проверяет строку на пустоту, комментарий или ошибку форматирования
static int normalize_line(char *line, int lineno) {
    size_t L = strlen(line);
    while (L > 0 && (line[L-1] == '\n' || line[L-1] == '\r')) line[--L] = '\0';
    if (line[0] == '\0') return 1;
    if (line[0] == '/' && line[1] == '/') return 1;
    if (line[0] == ' ' || line[0] == '\t') {
        printf("Ошибка формата (строка %d): левые пробелы недопустимы.\n", lineno);
        return -1;
    }
    return 0;
}

// Проверяет, что первая команда — SIZE или LOAD
static int check_first_command_is_valid(const char *line, int lineno) {
    char cmd[32] = {0};
    if (sscanf(line, "%31s", cmd) != 1) return -1;
    if (!strcmp(cmd, "SIZE") || !strcmp(cmd, "LOAD")) return 0;
    printf("Ошибка (строка %d): первая команда должна быть SIZE или LOAD.\n", lineno);
    return -1;
}

// Основная обработка команд из файла
int process_file(const char *filename, int is_nested) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Не удалось открыть файл '%s': %s\n", filename, strerror(errno));
        return -1;
    }
    char line[512];
    int lineno = 0, first_checked = 0;

    while (fgets(line, sizeof(line), f)) {
        lineno++;
        int norm = normalize_line(line, lineno);
        if (norm == 1) continue;
        if (norm < 0) { fclose(f); return -1; }
        if (!first_checked) {
            if (check_first_command_is_valid(line, lineno) < 0) {
                fclose(f);
                return -1;
            }
            first_checked = 1;
        }
        int rc = exec_command_line(line, lineno);
        if (rc < 0) { fclose(f); return -1; }
        if (!opt_no_display) {
            clearConsole();
            printField();
            sleepSeconds(opt_interval_sec);
        }
    }
    fclose(f);
    return 0;
}

// Разбирает дополнительные параметры запуска
static int parse_options(int argc, char **argv) {
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i], "no-display")) opt_no_display = 1;
        else if (!strcmp(argv[i], "no-save")) opt_no_save = 1;
        else if (!strcmp(argv[i], "interval")) {
            if (i + 1 >= argc) { printf("Опция interval требует число.\n"); return -1; }
            opt_interval_sec = atoi(argv[++i]);
            if (opt_interval_sec <= 0) opt_interval_sec = 1;
        } else {
            printf("Неизвестная опция: %s\n", argv[i]);
            return -1;
        }
    }
    return 0;
}

// Создаёт путь для сохранения результата
static void build_output_path(char *dst, size_t dst_sz, const char *basename) {
    snprintf(dst, dst_sz, "output/%s", basename);
}

// Главная функция программы
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Использование:\n  %s input.txt output.txt [опции]\n", argv[0]);
        printf("Опции: interval N, no-display, no-save\n");
        return 1;
    }
    const char *input_path = argv[1];
    const char *output_file = argv[2];
    if (parse_options(argc, argv) < 0) return 1;

    initFieldGlobals();
    undo_init();
    makeOutputDir("output");

    if (process_file(input_path, 0) < 0) return 1;

    if (!opt_no_save) {
        char out_path[512];
        build_output_path(out_path, sizeof(out_path), output_file);
        if (saveFieldToFile(out_path) < 0)
            printf("Ошибка сохранения '%s'.\n", out_path);
        else
            printf("Сохранено: %s\n", out_path);
    }
    return 0;
}
