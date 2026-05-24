#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fts.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>

#define VERSION "1.0"
#define AUTHOR "Жук Анастасия Валерьевна"
#define GROUP "N3248"
#define LAB_NAME "ОСП ЛР1.1"

// Структура для хранения байтовой последовательности
typedef struct {
    unsigned char *bytes;
    size_t length;
} ByteLine;

// Функция для преобразования строки с hex в байтовую последовательность
int hex_to_bytes(const char *str, ByteLine *line) {
    // Проверка входной строки
    if (str == NULL || line == NULL || strncmp(str, "0x", 2) != 0) {
        return -1;
    }

    size_t len = strlen(str) - 2;
    if (len == 0 || len % 2 != 0) {
        return -1;
    }

    line->length = len / 2;
    line->bytes = malloc(line->length);
    if (line->bytes == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        return -1;
    }

    // Осуществляем преобразование
    for (size_t i = 0; i < line->length; i++) {
        char byte_str[3] = {str[2 + i*2], str[2 + i*2 + 1], '\0'};
        char *endptr;
        long byte = strtol(byte_str, &endptr, 16);
        
        if (*endptr != '\0' || byte < 0 || byte > 255) {
            fprintf(stderr, "Ошибка: некорректное задание hex-строки\n");
            free(line->bytes);
            return -1;
        }
        
        line->bytes[i] = (unsigned char)byte;
    }

    return 0;
}

// Функция для проверки, содержит ли файл заданную последовательность
int line_in_file(const char *path, const ByteLine *line) {
    // Проверки входных данных
    if (path == NULL || line == NULL || line->length == 0) {
        return 0;
    }

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        if (getenv("LAB11DEBUG") != NULL) {
            fprintf(stderr, "Ошибка: не удалось открыть файл %s: %s\n", path, strerror(errno));
        }
        return 0;
    }

    unsigned char *buffer = malloc(line->length);
    if (buffer == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        close(fd);
        return 0;
    }

    int found = 0;
    ssize_t bytes_read;
    off_t offset = 0;

    while (bytes_read = pread(fd, buffer, line->length, offset)) {
        if (bytes_read == -1) {
            if (getenv("LAB11DEBUG") != NULL) {
                fprintf(stderr, "Ошибка: не удалось прочесть содеримое файла %s: %s\n", path, strerror(errno));
            }
            break;
        }

        if (bytes_read < (ssize_t)line->length) {
            break; // Недостаточно данных для сравнения
        }

        if (memcmp(buffer, line->bytes, line->length) == 0) {
            found = 1;
            if (getenv("LAB11DEBUG") != NULL) {
                fprintf(stderr, "Нашлась последовательность в файле %s со смещением %ld\n", path, offset);
            }
            break;
        }

        offset++;
    }

    free(buffer);
    close(fd);
    return found;
}

// Функция для обработки каталогов
void walk_function(const char *dir_path, const ByteLine *line) {
    // Подготавливаем массив путей для обхода
    char *paths[] = {(char *)dir_path, NULL};
    
    // Открываем поток для обхода файловой системы
    FTS *fts_h = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (!fts_h) {
        fprintf(stderr, "Ошибка: не удалось открыть директорию %s: %s\n", 
               dir_path, strerror(errno));
        return;
    }

    FTSENT *ent;  // Указатель на информацию о текущем файле
    while ((ent = fts_read(fts_h)) != NULL) {
        // Обрабатываем только обычные файлы
        if (ent->fts_info == FTS_F) {
            // Проверяем содержимое файла на совпадение
            if (line_in_file(ent->fts_path, line)) {
                // Выводим полный путь к найденному файлу
                printf("Найден файл: %s\n", ent->fts_path);
            }
        } 
        // Обрабатываем ошибки доступа
        else if (ent->fts_info == FTS_DNR || ent->fts_info == FTS_ERR) {
            fprintf(stderr, "Ошибка: не получилось получить доступ к %s: %s\n", ent->fts_path, strerror(ent->fts_errno));
        }
    }

    // Закрываем поток
    fts_close(fts_h);
}

// Функция для вывода справки
void print_help() {
    printf("Usage: lab11abcNXXXXX [options] directory byte_sequence\n");
    printf("Options:\n");
    printf("  -h, --help     Display this help message\n");
    printf("  -v, --version  Display version information\n");
    printf("\n");
    printf("The byte_sequence should be in the format 0xhh[hh...] where hh are hexadecimal digits\n");
    printf("Example: lab11abcNXXXXX /home 0xc0ffee\n");
}

// Функция для вывода версии
void print_version() {
    printf("%s\n", LAB_NAME);
    printf("Version: %s\n", VERSION);
    printf("Author: %s\n", AUTHOR);
    printf("Group: %s\n", GROUP);
}

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hv", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'v':
                print_version();
                return 0;
            default:
                print_help(); // ??? По идеи, написать, что присутствуют неподдерживаемые опции
                return 1;
        }
    }

    if (argc - optind < 2) {
        fprintf(stderr, "Ошибка: отсутствуют необходимые аргументы\n"); // на русский перевести
        print_help(); // ???
        return 1;
    }

    const char *directory = argv[optind];
    const char *search_bytes = argv[optind + 1];

    ByteLine line;
    if (hex_to_bytes(search_bytes, &line)) {
        fprintf(stderr, "Ошибка: неверный формат последовательности байт. Ожидаемый формат: 0xhh[ххх...]\n");
        return 1;
    }

    if (getenv("LAB11DEBUG") != NULL) {
        fprintf(stderr, "Debug: searching for sequence: ");
        for (size_t i = 0; i < line.length; i++) {
            fprintf(stderr, "0x%02x ", line.bytes[i]);
        }
        fprintf(stderr, "\n");
    }

    walk_function(directory, &line);

    free(line.bytes);
    return 0;
}