#include <stdio.h>          
#include <stdbool.h>         
#include <stdlib.h>          
#include <string.h>          
#include <ctype.h>           
#include <errno.h>           

#define INITIAL_BUF_SIZE 100 

typedef struct Node {        
    int edge;               
    struct Node* prev;      
    struct Node* next;      
} Node;                     

typedef struct Deque {      
    Node* first;            
    Node* last;             
    int size;               
} Deque;                    

Deque* create_deque() {     
    Deque* dq = malloc(sizeof(Deque));  // sizeof + malloc + присваивание = 3
    if (!dq) return NULL;               // проверка условия + return = 2
    dq->first = dq->last = NULL;        // 2 обращения + 2 присваивания = 4
    dq->size = 0;                       // обращение + присваивание = 2
    return dq;                          // return = 1
}                                       // всего (всегда в худшем случае): 3 + 1 (только проверили условие) + 4 + 2 + 1 = 11

void push_back(Deque* dq, int value) {  
    Node* newNode = malloc(sizeof(Node));  // sizeof + malloc + присваивание = 3
    if (!newNode) return;                  // проверка условия + return = 2
    
    newNode->edge = value;                 // обращение + присваивание = 2
    newNode->next = NULL;                  // обращение + присваивание = 2
    newNode->prev = dq->last;              // 2 обращения + присваивание = 3

    if (dq->last) {                        // обращение + проверка условия = 2
        dq->last->next = newNode;          // 2 обращения + присваивание = 3
    } else {
        dq->first = newNode;               // обращение + присваивание = 2
    }
    dq->last = newNode;                    // обращение + присваивание = 2
    dq->size++;                            // обращение + инкремент (сложение + присваивание) = 3
}                                          // всего: 3 + 1 (только проверили условие) + 2 + 2 + 3 + 2 + 3 (худшая из ветвей if) + 2 + 3 = 21

void push_front(Deque* dq, int value) {  
    Node* newNode = malloc(sizeof(Node));  // malloc + sizeof + присваивание = 3
    if (!newNode) return;                  // проверка условия + return = 2
    
    newNode->edge = value;                 // обращение + присваивание = 2
    newNode->prev = NULL;                  // обращение + присваивание = 2
    newNode->next = dq->first;             // 2 обращения + присваивание = 3

    if (dq->first) {                       // обращение + проверка условия = 2
        dq->first->prev = newNode;         // 2 обращения + присваивание = 3
    } else {
        dq->last = newNode;                // обращение + присваивание = 2
    }
    dq->first = newNode;                   // обращение + присваивание = 2
    dq->size++;                            // обращение + инкремент = 3
}                                          // всего: 3 + 1 (только условие) + 2 + 2 + 3 + 2 + 3 (худшая из ветвей if) + 2 + 3 = 21

int pop_front(Deque* dq) {  
    if (!dq->first) return -1;    // обращение + проверка условия + return = 3

    Node* temp = dq->first;       // обращение + присваивание = 2
    int value = temp->edge;       // обращение + присваивание = 2
    dq->first = dq->first->next;  // 3 обращения + присваивание = 4

    if (dq->first) {              // обращение + проверка условия = 2
        dq->first->prev = NULL;   // 2 обращения + присваивание = 3
    } else {
        dq->last = NULL;          // обращение + присваивание = 2
    }

    free(temp);                   // free = 1
    dq->size--;                   // обращение + декремент = 3
    return value;                 // return = 1
}                                 // всего: 2 (условие) + 2 + 2 + 4 + 2 (условие) + 3 (худшая из ветвей if) + 1 + 3 + 1 = 20

int pop_back(Deque* dq) {
    if (!dq->first) return -1;  // обращение + проверка условия + return = 3

    Node* temp = dq->last;      // обращение + присваивание = 2
    int value = temp->edge;     // обращение + присваивание = 2
    dq->last = dq->last->prev;  // 3 обращение + присваивание = 4

    if (dq->last) {             // обращение + проверка условия = 2
        dq->last->next = NULL;  // 2 обращение + присваивание = 3
    } else {
        dq->first = NULL;       // обращение + присваивание = 2
    }

    free(temp);                 // free = 1
    dq->size--;                 // обращение + декремент = 3
    return value;               // return = 1
}                               // всего: 2 (условие) + 2 + 2 + 4 + 2 (условие) + 3 (худшая из ветвей if) + 1 + 3 + 1 = 20

void free_deque(Deque* dq) {
    while (dq->first) {       // обращение + проверка условия = 2 * n^2 + 2 (где nxn - размерность матрицы смежности)
        pop_front(dq);        // pop_front = 20
    }                         // общее: (20 + 2) * N^2 + 2 = 22 * n^2 + 2
    free(dq);                 // free = 1
}                             // всего: 22 * n^2 + 3

bool is_empty(const Deque* dq) {
    return dq->size == 0;         // обращение + сравнение + return = 3
}                                 // всего: 3

int get_element(Deque* dq, int row, int col, int n) {
    Node* current = dq->first;                         // обращение + присваивание = 2
    int pos = row * n + col;                           // умножение + сложение + присваивание = 3
    
    for (int i = 0; i < pos && current; i++) {         // присваивание + (сравнение + логическое И + инкремент) = 5 (4 на каждой итерации)
        current = current->next;                       // обращение + присваивание = 2
    }                                                  // с учетом общего количества итераций: 1 + (4 + 2) * (row * n + col) + 2 (проверка условия для конца цикла) = (6 * row) * n + (6 * col + 3)
    return current ? current->edge : 0;                // обращение + условие + return = 3
}                                                      // всего: 2 + 3 + (6 * row) * n + (6 * col + 3) + 3 = (6 * row) * n + (6 * col + 11)
                                                       // худший случай: 6 * n^2 + 5 (так как row и col меньше n -> max_row = n - 1 и max_col = n - 1) 

int is_valid_number(const char *str) { 
    char *endptr;                       
    errno = 0;                                                        // присваивание = 1
    strtod(str, &endptr);                                             // вызов функции + амперсанд (взятие адреса) = 2
    
    return (*endptr == '\0' || isspace(*endptr)) && errno != ERANGE;  // разыменование + сравнение + логическое ИЛИ + разыменование + isspace + логическое И + сравнение + return = 8
}                                                                     // всего: 1 + 2 + 8 = 11

char* read_dynamic_line(FILE* file) {
    size_t buf_size = INITIAL_BUF_SIZE;                 // присваивание = 1
    char* buffer = malloc(buf_size);                    // malloc + присваивание = 2
    if (!buffer) return NULL;                           // проверка условия + return = 2

    size_t pos = 0;                                     // присваивание = 1
    int c;                               
    
    while ((c = fgetc(file)) != EOF && c != '\n') {     // fgetc + присваивание + 2 сравнения + логическое И = 5 (на каждой итерации)
        if (pos + 1 >= buf_size) {                      // сложение + сравнение = 2
            buf_size *= 2;                              // умножение + присваивание = 2
            char* new_buf = realloc(buffer, buf_size);  // realloc + присваивание = 2
            if (!new_buf) {                             // проверка условия = 1
                free(buffer);                           // free = 1
                return NULL;                            // return = 1
            }
            buffer = new_buf;                           // присваивание = 1
        }
        buffer[pos++] = (char)c;                        // индексация + инкремент + присваивание + приведение типа = 5
    }                                                   // общее (для цикла (длина считываемой строки - l)): l * (5 + 2 + 2 + 2 + 1 (условие) + 1 + 5) + 
                                                        // 5 (выход из цикла) = 18 * l + 5

    if (pos == 0 && c == EOF) {                         // 2 сравнения + логическое И = 3
        free(buffer);                                   // free = 1
        return NULL;                                    // return = 1
    }

    buffer[pos] = '\0';                                 // индексация + присваивание = 2
    return buffer;                                      // return = 1
}                                                       // всего: 1 + 2 + 2 + 1 + 18 * l + 5 + 3 (условие) + 2 + 1 = 18 * l + 17

int* kahn_topological_sort(Deque* matrix, int n, int* result_size) {
    int* in_degree = calloc(n, sizeof(int));       // sizeof + calloc + присваивание = 3
    if (!in_degree) {                              // проверка условия = 1
        printf("Ошибка...\n");                     // printf = 1
        return NULL;                               // return = 1
    }

    Deque* zero_in_degree = create_deque();        // вызов функции = 11
    if (!zero_in_degree) {                         // проверка условия = 1
        free(in_degree);                           // free = 1
        printf("Ошибка...\n");                     // printf = 1
        return NULL;                               // return = 1
    }

    int* result = malloc(n * sizeof(int));         // sizeof + умножение + malloc + присваивание = 4
    if (!result) {                                 // проверка условия = 1
        free(in_degree);                           // free = 1
        free_deque(zero_in_degree);                // вызов функции = 22 * n^2 + 3
        printf("Ошибка...\n");                     // printf = 1
        return NULL;                               // return = 1
    }

    *result_size = 0;                              // разыменование + присваивание = 2

    for (int i = 0; i < n; i++) {                  // инициализация + сравнение + инкремент = 3
        for (int j = 0; j < n; j++) {              // инициализация + сравнение + инкремент = 3
            if (get_element(matrix, i, j, n)) {    // вызов функции = 6 * n^2 + 5
                in_degree[j]++;                    // индексация + инкремент = 3
            }
        }
    }                                              // общее (для циклов): 1 + [1 + (6 * n^2 + 5 + 3) * n + 3n + 1] * n + 3n + 1 = 
                                                   // = 2 + 3n + 6 * n^4 + 11 * n^2 + 2n = 6 * n^4 + 11 * n^2 + 5n + 2

    for (int i = 0; i < n; i++) {                  // инициализация + сравнение + инкремент = 3
        if (in_degree[i] == 0) {                   // инкремент + сравнение = 2
            push_back(zero_in_degree, i);          // вызов функции = 21
        }
    }                                             // общее (для цикла): 1 + (2 + 21) * n + 1 = 23n + 2

    while (!is_empty(zero_in_degree)) {            // вызов функции + сравнение = 4
        int v = pop_front(zero_in_degree);         // вызов функции = 20
        result[(*result_size)++] = v;              // разыменование + инкремент + присваивание = 4

        for (int j = 0; j < n; j++) {              // инициализация + сравнение + инкремент = 3
            if (get_element(matrix, v, j, n)) {    // вызов функции = 6 * n^2 + 5
                in_degree[j]--;                    // индексация + декремент = 3
                if (in_degree[j] == 0) {           // индексация + сравнение = 2
                    push_back(zero_in_degree, j);  // вызов функции = 21
                }
            }
        }
    }                                              // общее (для цикла): [4 + 20 + 4 + 1 + 1 + (6 * n^2 + 5 + 3 + 2 + 21) * n] * n + 4 = 
                                                   // = 4 + [30 + 6 * n^3 + 31n] * n = 6 * n^4 + 31 * n^2 + 30n + 4

    free(in_degree);                               // free = 1
    free_deque(zero_in_degree);                    // вызов функции = 22 * n^2 + 3

    if (*result_size != n) {                       // разыменование + сравнение = 2
        free(result);                              // free = 1
        printf("Ошибка...\n");                     // printf = 1
        return NULL;                               // return = 1
    }

    return result;                                 // return = 1
}                                                  // всего: 3 + 1 + 11 + 1 + 4 + 1 + 2 + 6 * n^4 + 11 * n^2 + 5n + 2 + 23n + 2 + 6 * n^4 + 31 * n^2 + 30n + 4 + 1 +
                                                   // 22 * n^2 + 3 + 2 + 1 + 1 + 1 = 12 * n^4 + 64 * n^2 + 58n + 40 ~ n^4
                                                   // Вывод: сложность топологической сортировки по схеме Кана - O(n^4), где n - количество вершин графа

int kmp_search(const char* text, const char* pattern) {
    int n = strlen(text);                              // strlen + присваивание = 2
    int m = strlen(pattern);                           // strlen + присваивание = 2
    if (m == 0 || n < m) return -1;                    // 2 сравнения + логическое ИЛИ + return = 4

    int* lps = malloc(m * sizeof(int));                // sizeof + умножение + malloc + присваивание = 4
    if (!lps) return -1;                               // проверка условия + return = 2

    int len = 0;                                       // присваивание = 1
    lps[0] = 0;                                        // индексация + присваивание = 2
    
    for (int i = 1; i < m; ) {                         // инициализация + сравнение = 2
        if (pattern[i] == pattern[len]) {              // 2 индексации + сравнение = 3
            len++;                                     // инкремент = 2
            lps[i] = len;                              // индексация + присваивание = 2
            i++;                                       // инкремент = 2
        } else {
            if (len != 0) {                            // сравнение = 1
                len = lps[len - 1];                    // индексация + вычитание + присваивание = 3
            } else { 
                lps[i] = 0;                            // индексация + присваивание = 1
                i++;                                   // инкремент = 2
            }
        }
    }                                                  // общее (для цикла): 1 + ((3 + 2 + 2 + 2) + 1) * m = 10m + 1   

    int i = 0, j = 0;                                  // 2 присваивания = 2
    while (i < n) {                                    // сравнение = 1
        if (pattern[j] == text[i]) {                   // 2 индексации + сравнение = 3
            i++;                                       // инкремент = 2
            j++;                                       // инкремент = 2
        }
        if (j == m) {                                  // сравнение = 1
            free(lps);                                 // free = 1
            return i - j;                              // вычитание + return = 2
        } 
        else if (i < n && pattern[j] != text[i]) {     // 2 индексации + 2 сравнения + логическое И = 5
            if (j != 0) {                              // сравнение = 1
                j = lps[j - 1];                        // вычитание + индексация + присваивание = 3
            } else {
                i++;                                   // инкремент = 2
            }
        }
    }                                                   // общее (для цикла): (3 + 2 + 2 + 1 + 5 + 1 + 3) * n + 1 = 17n + 1

    free(lps);                                          // free = 1
    return -1;                                          // return = 1
}                                                       // всего: 2 + 2 + 3 + 4 + 1 + 1 + 2 + 10m + 1 + 2 + 17n + 1 + 1 + 1 = 17n + 10m + 21 ~ n + m
                                                        // Вывод: сложность поиска по схеме Кнута-Морриса-Пратта - O(n + m), где n - длина строки из отсортированных вершин графа,
                                                        // разделенных пробелами, а m - длина подстроки, которую нужно найти

char* read_input_line() {                               
    size_t buf_size = INITIAL_BUF_SIZE;                 // присваивание = 1
    char* buffer = malloc(buf_size);                    // malloc + присваивание = 2
    if (!buffer) return NULL;                           // проверка условия + return = 2

    size_t pos = 0;                                     // присваивание = 1
    int c;                                              
    
    while ((c = getchar()) != EOF && c != '\n') {       // getchar + присваивание + 2 сравнения + логическое И = 5
        if (pos + 1 >= buf_size) {                      // сложение + сравнение = 2
            buf_size *= 2;                              // умножение + присваивание = 2
            char* new_buf = realloc(buffer, buf_size);  // realloc + присваивание = 2
            if (!new_buf) {                             // проверка условия = 1
                free(buffer);                           // free = 1
                return NULL;                            // return = 1
            }
            buffer = new_buf;                           // присваивание = 1
        }
        buffer[pos++] = (char)c;                        // инкремент + индексация + приведение типа + присваивание = 4
    }

    if (pos == 0 && c == EOF) {                         // 2 сравнения + логическое И = 3
        free(buffer);                                   // free = 1
        return NULL;                                    // return = 1
    }

    buffer[pos] = '\0';                                 // индексация + присваивание = 2
    return buffer;                                      // return = 1
}                                                       // всего (k - длина считываемой строки): 1 + 2 + 2 + 1 + 1 + ((2 + 2 + 2 + 1 + 1) + 4) * k + 1 + 3 + 2 + 1 = 12k + 14

int main() {
    const char* filename = "matrix.txt";                                      // присваивание = 1
    FILE* file = fopen(filename, "r");                                        // fopen + присваивание = 2
    if (!file) {                                                              // проверка условия = 1
        perror("Ошибка: не удалось открыть файл");                            // perror = 1
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 3

    int rows = 0, cols = 0;                                                   // 2 присваивания = 2
    char* line = NULL;                                                        // присваивание = 1
    
    while ((line = read_dynamic_line(file)) != NULL) {                        // вызов функции (l - количество символов в строке файла) + присваивание + сравнение = 18 * l + 19
        if (strlen(line) == 0) {                                              // strlen + сравнение = 2
            free(line);                                                       // free = 1
            continue;                                                         // continue = 1
        }                                                                     // общее (для if): 4  

        int current_cols = 0;                                                 // присваивание = 1
        char* ptr = line;                                                     // присваивание = 1
        char* token;                        
        
        while (*ptr) {                                                        // разыменование + сравнение = 2 
            while (*ptr && isspace(*ptr)) ptr++;                              // 2 разыменования + 2 сравнения + логическое И + isspace + инкремент = 8
            if (!*ptr) break;                                                 // разыменование + сравнение + break = 3
            
            token = ptr;                                                      // присваивание = 1
            while (*ptr && !isspace(*ptr)) ptr++;                             // 2 разыменования + 2 сравнения + isspace + логическое И + инкремент = 8
            
            if (!is_valid_number(token)) {                                    // вызов функции = 11
                printf("Ошибка: некорректное значение '%.*s'\n", (int)(ptr - token), token);  // printf = 1
                free(line);                                                   // free = 1
                fclose(file);                                                 // fclose = 1
                return 1;                                                     // return = 1
            }
            current_cols++;                                                   // инкремент = 2
        }                                                                     // общее (для цикла (max_line - максимальная длина строки)): 5 + max_line * (2 + 8 + 2 + 1 + 45) = 58 * max_line + 5

        if (cols == 0) {                                                      // сравнение = 1
            cols = current_cols;                                              // присваивание = 1
        } else if (current_cols != cols) {                                    // сравнение = 1
            printf("Ошибка: разное количество элементов в строках\n");        // printf = 1
            free(line);                                                       // free = 1
            fclose(file);                                                     // fclose = 1
            return 1;                                                         // return = 1
        }                                                                     // общее (для if): 2 
        rows++;                                                               // инкремент = 2
        free(line);                                                           // free = 1
    }                                                                         // общее (для цикла): 4 + 58 * max_line + 5 + 2 + 3 = 58 * max_line + 14

    if (rows == 0 || cols == 0) {                                             // 2 сравнения + логическое ИЛИ = 3
        printf("Ошибка: пустая матрица\n");                                   // printf = 1
        fclose(file);                                                         // fclose = 1
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 6

    if (rows != cols) {                                                       // сравнение = 1
        printf("Ошибка: матрица смежности должна быть квадратной\n");         // printf = 1
        fclose(file);                                                         // fclose = 1
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 4

    rewind(file);                                                             // rewind = 1

    Deque* matrix = create_deque();                                           // вызов функции = 11
    if (!matrix) {                                                            // проверка условия = 1
        fclose(file);                                                         // fclose = 1
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 3

    while ((line = read_dynamic_line(file)) != NULL) {                        // присваивание + вызов функции (l - длина строки) + сравнение = 1 + 18 * l + 17 + 1 = 18 * l + 19
        if (strlen(line) == 0) {                                              // strlen + сравнение = 2
            free(line);                                                       // free = 1
            continue;                                                         // continue = 1
        }

        char* ptr = line;                                                     // присваивание = 1
        char* token;                     
        
        while (*ptr) {                                                        // разыменование + сравнение = 2
            while (*ptr && isspace(*ptr)) ptr++;                              // 2 разыменования + 2 сравнения + isspace + логическое И + инкремент = 8
            if (!*ptr) break;                                                 // разыменование + сравнение + break = 3
            
            token = ptr;                                                      // присваивание = 1
            while (*ptr && !isspace(*ptr)) ptr++;                             // 2 разыменования + 2 сравнения + isspace + логическое И + инкремент = 8
            
            double value = strtod(token, NULL);                               // strtod + присваивание = 2
            if (value != 0) push_back(matrix, 1);                             // сравнение + вызов функции = 1 + 21 = 22
            else push_back(matrix, 0);                                        // вызов функции = 21
        }
        free(line);                                                           // free = 1
    }                                                                         // общее (для цикла (max_line - максимальная длина строки)): 5 + max_line * (2 + 8 + 2 + 1 + 45) = 58 * max_line + 5
    fclose(file);                                                             // fclose = 1

    printf("Содержимое дека (матрица смежности графа):\n");                   // printf = 1
    Node* current = matrix->first;                                            // обращение + присваивание = 2
    for (int i = 0; i < rows; i++) {                                          // присваивание + сравнение + инкремент = 4
        for (int j = 0; j < cols; j++) {                                      // присваивание + сравнение + инкремент = 4
            if (current) {                                                    // проверка условия = 1
                printf("%d ", current->edge);                                 // обращение + printf = 2
                current = current->next;                                      // обращение + присваивание = 2
            }
        }
        printf("\n");                                                         // printf = 1
    }                                                                         // общее (для циклов): 1 + (4 + (1 + 8n + 1)) * n + 1 = 8 * n^2 + 6n + 2

    int result_size;                      
    int* sorted_vertices = kahn_topological_sort(matrix, rows, &result_size); // вызов функции = O(n^4)
    if (!sorted_vertices) {                                                   // проверка условия = 1
        free_deque(matrix);                                                   // вызов функции = 22 * n^2 + 3
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 22 * n^2 + 5
    
    char* sorted_str = malloc(rows * 12);                                     // умножение + malloc + присваивание = 3
    if (!sorted_str) {                                                        // проверка условия = 1
        printf("Ошибка: не удалось выделить память\n");                       // printf = 1
        free(sorted_vertices);                                                // free = 1
        free_deque(matrix);                                                   // вызов функции = 22 * n^2 + 3
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 1 + 3 + 22 * n^2 + 3 = 22 * n^2 + 7
    sorted_str[0] = '\0';                                                     // индексация + присваивание = 2

    for (int i = 0; i < result_size; i++) {                                   // присваивание + сравнение + инкремент = 4
        char num_str[12];                                                     
        sprintf(num_str, "%d ", sorted_vertices[i] + 1);                      // sprintf + индексация + сложение = 3
        strcat(sorted_str, num_str);                                          // strcat = 1
    }                                                                         // общее (для цикла): 1 + (1 + 3 + 3) * n + 1 = 7n + 2

    printf("Отсортированные вершины: %s\n", sorted_str);                      // printf = 1
    printf("Введите подстроку для поиска: ");                                 // printf = 1
    
    char* pattern = read_input_line();                                        // вызов функции = O(n)
    if (!pattern) {                                                           // проверка условия = 1
        printf("Ошибка: не удалось прочитать ввод\n");                        // printf = 1
        free(sorted_vertices);                                                // free = 1
        free(sorted_str);                                                     // free = 1
        free_deque(matrix);                                                   // вызов функции = 22 * n^2 + 3
        return 1;                                                             // return = 1
    }                                                                         // общее (для if): 1 + 3 + 1 + 22 * n^2 + 3 = 22 * n^2 + 8

    int pos = kmp_search(sorted_str, pattern);                                // вызов функции = O(n + m)
    if (pos != -1) {                                                          // сравнение = 1
        printf("Подстрока '%s' найдена на позиции: %d\n", pattern, pos + 1);  // printf = 1
    } else {
        printf("Подстрока '%s' не найдена\n", pattern);                       // printf = 1
    }                                                                         // общее (для if): 1 + 1 = 2

    free(sorted_vertices);                                                    // free = 1
    free(sorted_str);                                                         // free = 1
    free(pattern);                                                            // free = 1
    free_deque(matrix);                                                       // вызов функции = 22 * n^2 + 3

    return 0;                                                                 // return = 1
}                                                                             
                                                                              // Общая сложность: если просмотреть все общие сложности сложных кусков кода, то
                                                                              // можно заметить, что сложность программы будет O(n^4)