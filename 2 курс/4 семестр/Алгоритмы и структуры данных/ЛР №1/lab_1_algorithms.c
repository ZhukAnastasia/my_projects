#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Типы вершин для обхода в глубину
#define WHITE 0
#define GRAY 1
#define BLACK 2

#define MAX_STATIC_SIZE 100 // Максимальный размер статической матрицы (MAX_STATIC_SIZE х MAX_STATIC_SIZE) 
#define INITIAL_BUF_SIZE 1000 // Размер динамического массива, в который считываются строки из файла

// Проверка: является ли строка корректным вещественным числом
// Возвращает 1 (True), если строка является числом, и 0 (False) в противном случае
int is_valid_double(const char *str) {
    char *endptr; // Указатель на первый символ, который не удалось преобразовать в число
    errno = 0; // Код ошибки (0 - все хорошо)
    strtod(str, &endptr); // Перевод строки в число

    // Проверяем два условия валидности:
    // 1. Что преобразование дошло до конца строки (\0) или до пробельного символа
    // 2. Что не произошло переполнения (errno не установлен в ERANGE)
    return (*endptr == '\0' || isspace(*endptr)) && errno != ERANGE;
}

// Считываем строку из файла. Так как она может быть любого размера, то делаем считывание в динамический массив
char* read_dynamic_line(FILE* file) {
    size_t buf_size = INITIAL_BUF_SIZE;
    char* buffer = malloc(buf_size); // Здесь будет лежать считанная из файла строка (одна текущая)
    if (!buffer) return NULL; // Обрабатываем ошибку невозможности выделения памяти

    size_t pos = 0; // Позиция символа в массиве buffer
    int c; // Будет хранить код считываемого символа из таблицы ASCII

    while ((c = fgetc(file)) != EOF && c != '\n') {
        // Если нам не хватило места в массиве для всей строки, то нужно его увеличить
        if (pos + 1 >= buf_size) {
            buf_size *= 2;
            char* new_buf = realloc(buffer, buf_size);
            // Обрабатываем ошибку невозможности выделения памяти
            if (!new_buf) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }
        buffer[pos++] = (char)c; // Положили в массив считанный символ
    }

    if (pos == 0 && c == EOF) {
        free(buffer); // Если строка пустая, то освобождаем выделенную память
        return NULL;
    }

    buffer[pos] = '\0'; // Добавляем конец строки
    return buffer; // Возвращаем считанную строку
}

void dfs_static(int v, int n, double matrix[MAX_STATIC_SIZE][MAX_STATIC_SIZE], int* colors, int* result, int* index, bool* has_cycle) {
    colors[v] = GRAY; // Красим рассматриваемую вершину в серый
    
    for (int u = 0; u < n; u++) {
        if (matrix[v][u]) {
            if (colors[u] == WHITE) { // Если можем попасть в белую вершину, то идем в нее (рекурсивный обход в глубину)
                dfs_static(u, n, matrix, colors, result, index, has_cycle);
            } else if (colors[u] == GRAY) { // Если нам попалась серая вершина, значит мы ее уже рассматривали, а значит в графе цикл
                *has_cycle = true;
            }
        }
    }
    
    colors[v] = BLACK; // Красим рассмотренную вершину в черный, чтобы больше в нее не заходить, так как все пути из нее мы рассмотрели
    result[(*index)++] = v + 1; // Нумерация вершин с единицы
}

void topological_sort_static(int n, double matrix[MAX_STATIC_SIZE][MAX_STATIC_SIZE]) {
    int* colors = (int*)malloc(n * sizeof(int));
    int* result = (int*)malloc(n * sizeof(int));
    int index = 0;
    bool has_cycle = false;
    
    if (!colors || !result) {
        printf("Ошибка: не удалось выделить память\n");
        if (colors) free(colors);
        if (result) free(result);
        return;
    }
    
    for (int i = 0; i < n; i++) {
        colors[i] = WHITE; // Все вершины графа изначально белые (то есть подлежат рассмотрению)
    }
    
    for (int i = 0; i < n; i++) {
        if (colors[i] == WHITE) { // Осуществляем сортировку через обход в глубину
            dfs_static(i, n, matrix, colors, result, &index, &has_cycle);
        }
    }
    
    if (has_cycle) {
        printf("Ошибка: граф содержит циклы, топологическая сортировка невозможна\n");
    } else {
        printf("Топологический порядок вершин: [");
        for (int i = n - 1; i >= 0; i--) {
            printf("%d", result[i]);
            if (i > 0) printf(", ");
        }
        printf("]\n");
    }
    
    free(colors);
    free(result);
}

void dfs_dynamic(int v, int n, double** matrix, int* colors, int* result, int* index, bool* has_cycle) {
    colors[v] = GRAY; // Красим рассматриваемую вершину в серый
    
    for (int u = 0; u < n; u++) {
        if (matrix[v][u]) {
            if (colors[u] == WHITE) { // Если можем попасть в белую вершину, то идем в нее (рекурсивный обход в глубину)
                dfs_dynamic(u, n, matrix, colors, result, index, has_cycle);
            } else if (colors[u] == GRAY) { // Если нам попалась серая вершина, значит мы ее уже рассматривали, а значит в графе цикл
                *has_cycle = true;
            }
        }
    }
    
    colors[v] = BLACK; // Красим рассмотренную вершину в черный, чтобы больше в нее не заходить, так как все пути из нее мы рассмотрели
    result[(*index)++] = v + 1; // Нумерация вершин с единицы
}

void topological_sort_dynamic(int n, double** matrix) {
    int* colors = (int*)malloc(n * sizeof(int));
    int* result = (int*)malloc(n * sizeof(int));
    int index = 0;
    bool has_cycle = false;
    
    if (!colors || !result) {
        printf("Ошибка: не удалось выделить память\n");
        if (colors) free(colors);
        if (result) free(result);
        return;
    }
    
    for (int i = 0; i < n; i++) {
        colors[i] = WHITE; // Все вершины графа изначально белые (то есть подлежат рассмотрению)
    }
    
    for (int i = 0; i < n; i++) {
        if (colors[i] == WHITE) { // Осуществляем сортировку через обход в глубину
            dfs_dynamic(i, n, matrix, colors, result, &index, &has_cycle);
        }
    }
    
    if (has_cycle) {
        printf("Ошибка: граф содержит циклы, топологическая сортировка невозможна\n");
    } else {
        printf("Топологический порядок вершин: [");
        for (int i = n - 1; i >= 0; i--) {
            printf("%d", result[i]);
            if (i > 0) printf(", ");
        }
        printf("]\n");
    }
    
    free(colors);
    free(result);
}

int main() {
    int choice;
    printf("В какой массив записать матрицу? (1 - статический, 2 - динамический): ");
    scanf("%d", &choice);
    if (choice != 1 && choice != 2) {
        printf("Ошибка: некорректный тип массива\n");
        return 1;
    }

    const char *filename = "matrix.txt"; // Будем считывать только из одного файла

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка: не удалось открыть файл");
        return 1;
    }

    // Первый проход для оценки содержимого файла и определения размеров матрицы
    int rows = 0, cols = 0;
    char* line = NULL;
    
    while ((line = read_dynamic_line(file)) != NULL) {
        if (strlen(line) == 0) { // Встретили пустую строку
            free(line);
            continue;
        }

        int current_cols = 0; // Счетчик столбцов в текущей строке
        char *ptr = line; // Указатель для перемещения по строке
        char *token; // Указатель на начало текущего числа
        
        while (*ptr) { // Пока не конец строки
            // Пропускаем пробельные символы
            while (*ptr && isspace(*ptr)) ptr++;
            if (!*ptr) break; // Если строка закончилась, то выходим
            
            token = ptr; // Запоминаем начало числа
            while (*ptr && !isspace(*ptr)) ptr++; // Пропускаем все значимые символы (до конца числа)
            
            if (!is_valid_double(token)) { // Проверка, что считанный элемент матрицы не является числом
                printf("Ошибка: некорректное значение '%.*s'\n", (int)(ptr - token), token);
                free(line);
                fclose(file);
                return 1;
            }
            
            current_cols++;
        }

        if (cols == 0) cols = current_cols;
        else if (current_cols != cols) {
            printf("Ошибка: разное количество элементов в строках\n");
            free(line);
            fclose(file);
            return 1;
        }
        rows++; // Одну строку обработали, идем к следующей
        free(line);
    }

    if (rows == 0 || cols == 0) {
        printf("Ошибка: пустая матрица\n");
        fclose(file);
        return 1;
    }

    //Ситуация, когда рассматриваемая матрица больше по размеру, чем статический массив
    if ((choice == 1) && (rows > MAX_STATIC_SIZE || cols > MAX_STATIC_SIZE)) {
        printf("Ошибка: матрица слишком большая для статического массива\n");
        fclose(file);
        return 1;
    }

    if (rows != cols) {
        printf("Ошибка: матрица смежности должна быть квадратной\n");
        fclose(file);
        return 1;
    }

    rewind(file);

    // Второй проход для записи данных в двумерный массив
    if (choice == 1) { // Считываем в статический массив
        double matrix[MAX_STATIC_SIZE][MAX_STATIC_SIZE];
        for (int i = 0; i < rows; i++) {
            line = read_dynamic_line(file);
            
            if (!line) {
                printf("Ошибка: не удалось прочитать строку %d\n", i + 1);
                fclose(file);
                return 1;
            }

            while (strlen(line) == 0) {
                line = read_dynamic_line(file);

                if (!line) {
                    printf("Ошибка: не удалось прочитать строку %d\n", i + 1);
                    fclose(file);
                    return 1;
                }
            }

            char *ptr = line;
            char *token;
            for (int j = 0; j < cols; j++) {
                while (*ptr && isspace(*ptr)) ptr++; // Пропускаем пробельные символы

                // Считываем число
                token = ptr;
                while (*ptr && !isspace(*ptr)) ptr++;

                // Если пользователь ввел матрицу весов, то заменим веса на единицы, то есть на обычные ребра (превратим матрицу весов в матрицу смежности)
                double token_double = (double)strtod(token, NULL);
                if (token_double == 0) matrix[i][j] = 0;
                else matrix[i][j] = 1;
            }
            free(line);
        }

        // Вывод матрицы смежности графа
        printf("Матрица %dx%d:\n", rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("%g ", matrix[i][j]);
            }
            printf("\n");
        }

        int n = rows;
        topological_sort_static(n, matrix);

    } else { // Считывание в динамический массив
        // Выделяем память для динамического массива
        double** matrix = (double**)malloc(rows * sizeof(double*));
        for (int i = 0; i < rows; i++) {
            matrix[i] = (double*)malloc(cols * sizeof(double));
        }

        for (int i = 0; i < rows; i++) {
            line = read_dynamic_line(file);
            
            if (!line) {
                printf("Ошибка: не удалось прочитать строку %d\n", i + 1);
                for (int k = 0; k < i; k++) free(matrix[k]);
                free(matrix);
                fclose(file);
                return 1;
            }

            while (strlen(line) == 0) {
                line = read_dynamic_line(file);

                if (!line) {
                    printf("Ошибка: не удалось прочитать строку %d\n", i + 1);
                    for (int k = 0; k < i; k++) free(matrix[k]);
                    free(matrix);
                    fclose(file);
                    return 1;
                }
            }

            char *ptr = line;
            char *token;
            for (int j = 0; j < cols; j++) {
                while (*ptr && isspace(*ptr)) ptr++; // Пропускаем пробельные символы
                
                // Считываем число
                token = ptr;
                while (*ptr && !isspace(*ptr)) ptr++;
                
                // Если пользователь ввел матрицу весов, то заменим веса на единицы, то есть на обычные ребра (превратим матрицу весов в матрицу смежности)
                double token_double = (double)strtod(token, NULL);
                if (token_double == 0) matrix[i][j] = 0;
                else matrix[i][j] = 1;
            }
            free(line);
        }

        // Вывод матрицы смежности графа
        printf("Матрица %dx%d:\n", rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("%g ", matrix[i][j]);
            }
            printf("\n");
        }

        int n = rows;
        topological_sort_dynamic(n, matrix);

        for (int i = 0; i < rows; i++) free(matrix[i]);
        free(matrix);
    }

    fclose(file);
    return 0;
}