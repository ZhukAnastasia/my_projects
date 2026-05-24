#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

// Начальный размер буфера для чтения строк
#define INITIAL_BUF_SIZE 100

// Структура узла двусвязного списка
typedef struct Node {
    int edge; // Значение ребра (0 или 1)
    struct Node* prev; // Указатель на предыдущий узел
    struct Node* next; // Указатель на следующий узел
} Node;

// Структура дека (двусторонней очереди)
typedef struct Deque {
    Node* first; // Указатель на начало дека
    Node* last; // Указатель на конец дека
    int size; // Текущее количество элементов
} Deque;

/*
Создает новый пустой дек
Возвращает указатель на созданный дек или NULL при ошибке выделения памяти
*/
Deque* create_deque() {
    Deque* dq = malloc(sizeof(Deque));
    if (!dq) return NULL; // Проверка успешности выделения памяти
    dq->first = dq->last = NULL; // Инициализация пустого дека
    dq->size = 0;
    return dq;
}

/*
Добавляет элемент в конец дека
dq - указатель на дек
value - значение для добавления
*/
void push_back(Deque* dq, int value) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) return; // Проверка выделения памяти
    
    // Инициализация нового узла
    newNode->edge = value;
    newNode->next = NULL;
    newNode->prev = dq->last;

    // Обновление связей в деке
    if (dq->last) {
        dq->last->next = newNode; // Если дек не пуст
    } else {
        dq->first = newNode; // Если дек пуст
    }
    dq->last = newNode; // Новый элемент теперь последний
    dq->size++; // Увеличиваем счетчик элементов
}

/*
Добавляет элемент в начало дека
dq - указатель на дек
value - значение для добавления
*/
void push_front(Deque* dq, int value) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) return; // Проверка выделения памяти
    
    // Инициализация нового узла
    newNode->edge = value;
    newNode->prev = NULL;
    newNode->next = dq->first;

    if (dq->first) {
        dq->first->prev = newNode; // Если дек не пуст
    } else {
        dq->last = newNode; // Если дек пуст
    }
    dq->first = newNode;
    dq->size++;
}

/*
Удаляет элемент из начала дека
dq - указатель на дек
Возвращает значение удаленного элемента или -1, если дек пуст
 */
int pop_front(Deque* dq) {
    if (!dq->first) return -1; // Проверка на пустоту дека

    Node* temp = dq->first; // Сохраняем ссылку на удаляемый узел
    int value = temp->edge; // Сохраняем значение
    dq->first = dq->first->next; // Перемещаем указатель на следующий элемент

    // Обновляем связи
    if (dq->first) {
        dq->first->prev = NULL; // Удаляем ссылку на предыдущий элемент
    } else {
        dq->last = NULL; // Если дек стал пустым
    }

    free(temp); // Освобождаем память
    dq->size--; // Уменьшаем счетчик элементов
    return value; // Возвращаем значение
}

/*
Удаляет элемент из конца дека
dq - указатель на дек
Возвращает значение удаленного элемента или -1, если дек пуст
 */
int pop_back(Deque* dq) {
    if (!dq->first) return -1; // Проверка на пустоту дека

    Node* temp = dq->last; // Сохраняем ссылку на удаляемый узел
    int value = temp->edge; // Сохраняем значение
    dq->last = dq->last->prev; // Перемещаем указатель на следующий элемент

    // Обновляем связи
    if (dq->last) {
        dq->last->next = NULL; // Удаляем ссылку на предыдущий элемент
    } else {
        dq->first = NULL; // Если дек стал пустым
    }

    free(temp); // Освобождаем память
    dq->size--; // Уменьшаем счетчик элементов
    return value; // Возвращаем значение
}

/*
Освобождает память, занятую деком
dq - указатель на дек
*/
void free_deque(Deque* dq) {
    while (dq->first) { // Пока дек не пуст
        pop_front(dq); // Удаляем элементы из начала
    }
    free(dq); // Освобождаем память структуры дека
}

/*
Проверяет дек на пустоту
dq - указатель на дек
Возвращает true, если дек пуст, иначе false
*/
bool is_empty(const Deque* dq) {
    return dq->size == 0;
}

/*
Получает элемент матрицы по индексам строки и столбца
dq - указатель на дек с матрицей
row - индекс строки
col - индекс столбца
n - размер матрицы (n x n)
Возвращает значение элемента матрицы или 0 при ошибке
*/
int get_element(Deque* dq, int row, int col, int n) {
    Node* current = dq->first;
    int pos = row * n + col; // Линейная позиция в деке
    
    // Переход к нужной позиции
    for (int i = 0; i < pos && current; i++) {
        current = current->next;
    }
    return current ? current->edge : 0; // Возвращаем значение или 0 если NULL
}

/*
Проверяет, является ли строка корректным числом
str - проверяемая строка
Возвращает 1, если строка - корректное число, иначе 0
*/
int is_valid_number(const char *str) {
    char *endptr;
    errno = 0;
    strtod(str, &endptr); // Пытаемся преобразовать строку в число
    
    // Проверяем два условия:
    // 1. Преобразование дошло до конца строки или пробела
    // 2. Не было переполнения
    return (*endptr == '\0' || isspace(*endptr)) && errno != ERANGE;
}

// Считываем строку из файла

/*
Читает строку произвольной длины из файла
file - указатель на файл
Возвращает указатель на прочитанную строку или NULL при ошибке
*/
char* read_dynamic_line(FILE* file) {
    size_t buf_size = INITIAL_BUF_SIZE;
    char* buffer = malloc(buf_size);
    if (!buffer) return NULL; // Проверка выделения памяти

    size_t pos = 0;
    int c;
    
    // Читаем символы до конца строки или файла
    while ((c = fgetc(file)) != EOF && c != '\n') {
        // Если буфер заполнен, увеличиваем его размер
        if (pos + 1 >= buf_size) {
            buf_size *= 2;
            char* new_buf = realloc(buffer, buf_size);
            if (!new_buf) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }
        buffer[pos++] = (char)c; // Записываем символ в буфер
    }

    // Если ничего не прочитали и достигли конца файла
    if (pos == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }

    buffer[pos] = '\0'; // Добавляем завершающий нулевой символ
    return buffer;
}

// Алгоритм топологической сортировки по схеме Кана

/*
Выполняет топологическую сортировку графа алгоритмом Кана
adj_matrix - матрица смежности графа в виде дека
n - размер матрицы (количество вершин)
result_size - указатель для сохранения размера результата
Возвращает массив с отсортированными вершинами или NULL при наличии цикла
*/
int* kahn_topological_sort(Deque* matrix, int n, int* result_size) {
    // Выделяем память для полустепеней захода
    int* in_degree = calloc(n, sizeof(int));
    if (!in_degree) {
        printf("Ошибка: не удалось выделить память внутри сортировки\n");
        return NULL; // Не удалось выделить память
    }

    // Создаём очередь вершин с нулевой полустепенью
    Deque* zero_in_degree = create_deque();
    if (!zero_in_degree) {
        free(in_degree);
        printf("Ошибка: не удалось создать дек внутри сортировки\n");
        return NULL; // Не удалось создать дек
    }

    // Выделяем память для результата
    int* result = malloc(n * sizeof(int));
    if (!result) {
        free(in_degree);
        free_deque(zero_in_degree);
        printf("Ошибка: не удалось выделить память внутри сортировки\n");
        return NULL; // Не удалось выделить память
    }

    *result_size = 0; // Изначально результат пуст

    // Вычисляем полустепени захода для всех вершин
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (get_element(matrix, i, j, n)) {
                in_degree[j]++;
            }
        }
    }

    // Добавляем в очередь вершины с нулевой полустепенью
    for (int i = 0; i < n; i++) {
        if (in_degree[i] == 0) {
            push_back(zero_in_degree, i);
        }
    }

    // Основной цикл алгоритма
    while (!is_empty(zero_in_degree)) {
        int v = pop_front(zero_in_degree);
        result[(*result_size)++] = v;

        // Уменьшаем полустепени захода для всех соседей
        for (int j = 0; j < n; j++) {
            if (get_element(matrix, v, j, n)) {
                in_degree[j]--;
                if (in_degree[j] == 0) {
                    push_back(zero_in_degree, j);
                }
            }
        }
    }

    // Освобождаем память
    free(in_degree);
    free_deque(zero_in_degree);

    // Проверяем, есть ли циклы в графе
    if (*result_size != n) {
        free(result);
        printf("Ошибка: граф содержит цикл, топологическая сортировка невозможна\n");
        return NULL; // Граф содержит цикл
    }

    return result;
}

// Алгоритм Кнута-Морриса-Пратта

/*
Выполняет поиск подстроки в тексте алгоритмом КМП
text - текст для поиска
pattern - искомая подстрока
Возвращает позицию начала подстроки или -1, если не найдено
*/
int kmp_search(const char* text, const char* pattern) {
    int n = strlen(text); // Длина текста
    int m = strlen(pattern); // Длина образца
    if (m == 0 || n < m) return -1; // Проверка на пустые строки

    // Создаем массив для префикс-функции
    int* lps = malloc(m * sizeof(int));
    if (!lps) return -1; // Проверка выделения памяти

    // Вычисляем префикс-функцию для образца
    int len = 0; // Длина текущего префикса
    lps[0] = 0; // Префикс-функция для первого символа всегда 0
    
    for (int i = 1; i < m; ) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }

    // Поиск образца в тексте
    int i = 0, j = 0; // Индексы для текста и образца
    while (i < n) {
        if (pattern[j] == text[i]) { // Совпадение символов
            i++;
            j++;
        }
        // Если нашли полное совпадение
        if (j == m) {
            free(lps);
            return i - j; // Возвращаем позицию начала совпадения
        } 
        // Если символы не совпали
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1]; // Используем префикс-функцию для сдвига
            } else {
                i++; // Просто двигаемся по тексту
            }
        }
    }

    free(lps);
    return -1; // Совпадение не найдено
}

// Чтение пользовательского ввода

/*
Читает строку с пробелами из стандартного ввода
Возвращает указатель на прочитанную строку или NULL при ошибке
*/
char* read_input_line() {
    size_t buf_size = INITIAL_BUF_SIZE;
    char* buffer = malloc(buf_size);
    if (!buffer) return NULL; // Проверка выделения памяти

    size_t pos = 0;
    int c;
    
    // Читаем символы до конца строки или файла
    while ((c = getchar()) != EOF && c != '\n') {
        // Если буфер заполнен, увеличиваем его размер
        if (pos + 1 >= buf_size) {
            buf_size *= 2;
            char* new_buf = realloc(buffer, buf_size);
            if (!new_buf) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }
        buffer[pos++] = (char)c; // Записываем символ в буфер
    }

    // Если ничего не прочитали и достигли конца файла
    if (pos == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }

    buffer[pos] = '\0'; // Добавляем завершающий нулевой символ
    return buffer;
}

int main() {
    // Начинаем считать время
    //clock_t start_time = clock();

    const char* filename = "matrix.txt"; // Имя файла с матрицей смежности
    FILE* file = fopen(filename, "r"); // Открываем файл для чтения
    if (!file) {
        perror("Ошибка: не удалось открыть файл");
        return 1;
    }

    // Чтнение и проверка матрицы
    
    int rows = 0, cols = 0; // Счетчики строк и столбцов
    char* line = NULL; // Буфер для чтения строк
    
    // Первый проход: определяем размеры матрицы
    while ((line = read_dynamic_line(file)) != NULL) {
        if (strlen(line) == 0) { // Пропускаем пустые строки
            free(line);
            continue;
        }

        // Разбираем строку на числа
        int current_cols = 0;
        char* ptr = line;
        char* token;
        
        while (*ptr) {
            // Пропускаем пробелы
            while (*ptr && isspace(*ptr)) ptr++;
            if (!*ptr) break; // Конец строки
            
            // Выделяем токен (число)
            token = ptr;
            while (*ptr && !isspace(*ptr)) ptr++;
            
            // Проверяем, является ли токен числом
            if (!is_valid_number(token)) {
                printf("Ошибка: некорректное значение '%.*s'\n", (int)(ptr - token), token);
                free(line);
                fclose(file);
                return 1;
            }
            current_cols++;
        }

        // Проверяем согласованность количества столбцов
        if (cols == 0) {
            cols = current_cols; // Первая строка задает количество столбцов
        } else if (current_cols != cols) {
            printf("Ошибка: разное количество элементов в строках\n");
            free(line);
            fclose(file);
            return 1;
        }
        rows++; // Считаем строки
        free(line);
    }

    // Проверяем, что матрица не пустая
    if (rows == 0 || cols == 0) {
        printf("Ошибка: пустая матрица\n");
        fclose(file);
        return 1;
    }

    // Проверяем, что матрица квадратная
    if (rows != cols) {
        printf("Ошибка: матрица смежности должна быть квадратной\n");
        fclose(file);
        return 1;
    }

    // Возвращаемся в начало файла для второго прохода
    rewind(file);

    // Создание и заполнение дека
    
    // Создаем единый дек для хранения матрицы смежности
    Deque* matrix = create_deque();
    
    // Чтение матрицы и сохранение в дек
    while ((line = read_dynamic_line(file)) != NULL) {
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        char* ptr = line;
        char* token;
        
        while (*ptr) {
            while (*ptr && isspace(*ptr)) ptr++;
            if (!*ptr) break;
            
            token = ptr;
            while (*ptr && !isspace(*ptr)) ptr++;
            
            // Преобразуем строку в число и добавляем в дек
            double value = strtod(token, NULL);
            if (value != 0) push_back(matrix, 1); // Ребро есть
            else push_back(matrix, 0); // Ребра нет
        }
        free(line);
    }
    fclose(file);

    // Выводим содержимое дека для демонстрации
    printf("Содержимое дека (матрица смежности графа):\n");
    Node* current = matrix->first;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (current) {
                printf("%d ", current->edge);
                current = current->next;
            }
        }
        printf("\n");
    }

    // Топологическая сортировка
    
    int result_size;
    int* sorted_vertices = kahn_topological_sort(matrix, rows, &result_size);

    // Проверяем, удалась ли сортировка
    if (!sorted_vertices) {
        free_deque(matrix);
        return 1;
    }
    
    // Преобразуем результат сортировки в строку
    char* sorted_str = malloc(rows * 12); // Выделяем память с запасом
    if (!sorted_str) {
        printf("Ошибка: не удалось выделить память\n");
        free(sorted_vertices);
        free_deque(matrix);
        return 1;
    }
    sorted_str[0] = '\0'; // Начинаем с пустой строки

    // Формируем строку из отсортированных вершин
    for (int i = 0; i < result_size; i++) {
        char num_str[12];
        sprintf(num_str, "%d ", sorted_vertices[i] + 1); // +1 для нумерации вершин с 1
        strcat(sorted_str, num_str); // Добавляем к результату
    }

    // Поиск подстроки
    
    printf("Отсортированные вершины: %s\n", sorted_str);
    printf("Введите подстроку для поиска: ");
    
    // Чтение ввода с пробелами
    char* pattern = read_input_line();
    if (!pattern) {
        printf("Ошибка: не удалось прочитать ввод\n");
        free(sorted_vertices);
        free(sorted_str);
        free_deque(matrix);
        return 1;
    }

    // Выполняем поиск и выводим результат
    int pos = kmp_search(sorted_str, pattern);
    if (pos != -1) {
        printf("Подстрока '%s' найдена на позиции: %d\n", pattern, pos + 1); // +1 для читаемого формата
    } else {
        printf("Подстрока '%s' не найдена\n", pattern);
    }

    // Освобождение памяти
    free(sorted_vertices);
    free(sorted_str);
    free(pattern);
    free_deque(matrix);

    // Заканчиваем считать время
    //clock_t end_time = clock();
    //double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000;
    //printf("Время выполнения программы: %.3f миллисекунд\n", elapsed_time);

    return 0;
}