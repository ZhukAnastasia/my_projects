#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

//виды ошибок
#define NO_ERROR 0
#define ERROR_MANY_ARGUMENTS 1
#define ERROR_INCORRECT_OPTIONS 2
#define ERROR_NOT_DIGITALS 3
#define ERROR_DONT_OPEN_FILE 4
#define ERROR_WRONG_ARGUMENTS 5

void infileall(char *, char *, int, int, int);
void infileonly(char *, char *, int, int, int);
void inconsolall(int, int, int);
void inconsolonly(int, int, int);

int main(int argc, char *argv[]) {

	char *DEBUG = getenv("LAB3DEBUG");
    	if (DEBUG) {
        	fprintf(stderr, "Включен вывод отладочных сообщений\n");
    	}

	int c = 0, b = 1, e = 0, n = 0; //c = 1, b = k, e = m, n = 1 
	char *txt1 = NULL;
	txt1 = (char*) malloc(sizeof(char));
	char *txt2 = NULL;
	txt2 = (char*) malloc(sizeof(char)); //txt1 = 1, txt2 = 1, no = 1
	int c1 = 0, c2 = 0, c3 = 0;
	errno = NO_ERROR;
	//проверка
	if (argc == 2 && strcmp(argv[1], "-v") == 0) {
		printf("Жук Анастасия Валерьевна гр. N3148\nВариант: 9-1-1-5\n");
		return errno;
	}
	else if (argc >= 2) {
		for (int i = 1; i < argc; i++) {
			if (strchr(argv[i], '.') != NULL) {
				if (strlen(txt1) == 0) {
					txt1 = (char*) realloc(txt1, strlen(argv[i]) * sizeof(char));
					memcpy(txt1, argv[i], strlen(argv[i]));
				}
				else {
					txt2 = (char*) realloc(txt2, strlen(argv[i]) * sizeof(char));
					memcpy(txt2, argv[i], strlen(argv[i]));
					c3++;
				}
			}
			else if (strcmp(argv[i], "-c") == 0) c++;
			else if (strcmp(argv[i], "-n") == 0) n++;
			else if (argv[i][0] == '-' && argv[i][1] == 'b') {
				if (sscanf(strchr(argv[i], '=') + 1, "%d", &b) == 0) {
					printf("Ошибка: ");
					for (int j = 3; j < (int) strlen(argv[i]); j++) {
						printf("%c", argv[i][j]);
					}
					printf(" не является числом!\n");
					return ERROR_NOT_DIGITALS;
				}
				c1++;
			}
			else if (argv[i][0] == '-' && argv[i][1] == 'e') {
				if (sscanf(strchr(argv[i], '=') + 1, "%d", &e) == 0) {
					printf("Ошибка: ");
					for (int j = 3; j < (int) strlen(argv[i]); j++) {
						printf("%c", argv[i][j]);
					}
					printf(" не является числом!\n");
					errno = ERROR_NOT_DIGITALS;
				}
				c2++;
			}
			else if (argv[i][0] == '-') {
				printf("Ошибка: неподдерживаемая опция ");
				for (int j = 0; j < (int) strlen(argv[i]); j++) {
					if (argv[i][j] == '=') break;
					printf("%c", argv[i][j]);
				}
				printf("!\n");
				errno = ERROR_INCORRECT_OPTIONS;
			}
			else {
				printf("Ошибка: некорректный агрумент %s!\n", argv[i]);
				errno = ERROR_WRONG_ARGUMENTS;
			}
		}
		if (c3 >= 2 || c >= 2 || n >= 2 || c1 >= 2 || c2 >= 2) {
			printf("Ошибка: слишком много аргументов!\n");
			errno = ERROR_MANY_ARGUMENTS;
		}		
	}
	
	if (errno == NO_ERROR) {
		if (strlen(txt1) != 0) {
			FILE *f = fopen(txt1, "r");
			if (!f) {
				printf("Ошибка: не удалось открыть файл %s!\n", txt1);
				errno = ERROR_DONT_OPEN_FILE;
				return errno;
			}
			else fclose(f);
			if (n == 0) infileall(txt1, txt2, c, b, e);
			else infileonly(txt1, txt2, c, b, e);
		}
		else {
			if (n != 0) inconsolonly(c, b, e);
			else inconsolall(c, b, e);
		}
	}
	return errno;
}
