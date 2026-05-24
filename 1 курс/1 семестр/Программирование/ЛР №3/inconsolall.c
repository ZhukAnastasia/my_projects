#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//обрабатывает все подходящие
void inconsolall(int cv, int bv, int ev) {
	//str - вводимая строка, str1 - вспомогательная строка, которая будет хранить в себе нужную нам подстроку (или что-то похожее на неё)
	char *str = NULL;
	char *str1 = NULL;
	str = (char*) malloc(sizeof(char));
	str1 = (char*) malloc(sizeof(char));
	//для считывания символов
	int t;
	char c;
	int i = 0, k = 0, r = 0; //i - текущая длина str, k - текущая длина str1
	int y = 0, g = 0; //счетчики
	char *arr = NULL; //нужна для того, чтобы хранить переносы строк между частями одной последовательности
	arr = (char*) malloc(sizeof(char));
	int w = 0; //вспомогательная переменная, чтобы записывать символы в строку str
	int flag = 1; //флаг для обработки последовательностей
	int countn = 1; //счетчик переносов строк
	while ((t = getchar()) != EOF) {
		//обработка переноса строки и других символов
		if (t == 13) continue;
		else if (t == 10) c = '\n';
		else c = (char) t;
		i++;
		str = (char*) realloc(str, i * sizeof(char)); //увеличим строку
		if ((bv == 1 || countn >= bv) && (ev == 0 || ev >= countn)) { //обработка конкретных строк (ограничения b и e)
			if ((isdigit(c) || c == '-' || (c == '\n' && k > 0 && k < 17))) { //будем здесь собирать строки с числами
				if (c == '\n') {
					countn++;
					r++;
					arr = (char*) realloc(arr, r * sizeof(char)); //увеличим строку
					arr[r - 1] = c;
				}
				else {
					k++;
					r++;
					str1 = (char*) realloc(str1, k * sizeof(char)); //увеличим строку
					str1[k - 1] = c;
					arr = (char*) realloc(arr, r * sizeof(char)); //увеличим строку
					arr[r - 1] = c;
					if (c == '-') y++; //счетчик символов '-'
					//обработка для того, чтобы правильно считывать последовательности типа "xxx..x\nxxx-x-xxx-xxxxx-x"
					if (k == 4 && c != '-') {
							for (int j = 0; j < k - 1; j++) str1[j] = str1[j + 1];
							str1[k - 1] = ' ';
							k--;
							str1 = (char*) realloc(str1, k * sizeof(char));
							flag = 0;
					}
					else if (k == 4 && c == '-' && r > 4 && arr[r - 5] == '\n') {
						for (int j = 0; j < r - 4; j++) {
							str[w] = arr[j];
							w++;
						}
						explicit_bzero(arr, 0);
						r = 4;
						arr = (char*) realloc(arr, r * sizeof(char)); 
						memcpy(arr, str1, 4);
						flag = 1;
					}
				}
			}
			else {
				if (c == '\n') countn++;
				if (y == 4 && k == 17 && (c == ' ' || c == '\n') && flag) { //проверка, что строка str1 вида ххх-х-ххх-ххххх-х
					char *str2 = NULL; //вспомогательная строка (её будем разбивать по "-")
					str2 = (char*) realloc(str2, k * sizeof(char));
					memcpy(str2, str1, k);
					char *p = strtok(str2, "-");
					int f = 1;
					while (p) {
						if (g == 0 && strlen(p) != 3) f = 0;
						else if (g == 1 && strlen(p) != 1) f = 0;
						else if (g == 2 && strlen(p) != 3) f = 0;
						else if (g == 3 && strlen(p) != 5) f = 0;
						g++;
						if (f == 0) break;
						else p = strtok(NULL, "-");
						
					}
					explicit_bzero(str2, 0);
					free(str2);
					if (f == 1) { //проверка по алгоритму ISBN-13
						int d = 0, l = 0;
						for (int j = 0; j < 15; j++) {
							if (j != 3 && j != 5 && j != 9) {
								l++; //для того, чтобы умножать на 3 только четные элементы
								d += 2 * ((l + 1) % 2) * ((int) str1[j] - 48) + (int) str1[j] - 48; //48 - код 0 в ASCII
							}
						}
						if (d % 10 != ((int) str1[16] - 48)) f = 0;
						else { //нужно увеличить строку str на 2 для того, чтобы можно было обособить подходящую нам str1, и вставить ее в исходную
							i += 2;
							str = (char*) realloc(str, i * sizeof(char));
							str[i - 2 - r - 1] = '*';
							memcpy(str + i - 2 - r, arr, r);
							str[i - 2] = '*';
							str[i - 1] = c;
							w += 3 + r;
							//очистим str1
							explicit_bzero(str1, 0);
							str1 = (char*) realloc(str1, sizeof(char));
							k = 0;
							//очистим arr
							explicit_bzero(arr, 0);
							arr = (char*) realloc(arr, sizeof(char));
							r = 0;
						}
					} 
					if (f == 0) {
						//записываем в str то, что лежит в str1 (так как так лежит то, что нам не подошло)
						memcpy(str + i - r - 1, arr, r);
						str[i - 1] = c;
						w += r + 1;
						//очистим str1
						explicit_bzero(str1, 0);
						str1 = (char*) realloc(str1, sizeof(char));
						k = 0;
						//очистим arr
						explicit_bzero(arr, 0);
						arr = (char*) realloc(arr, sizeof(char));
						r = 0;
					}
				}
				else { //записываем в str то, что лежит в str1 (так как так лежит то, что нам не подошло)
					memcpy(str + i - r - 1, arr, r);
					str[i - 1] = c;
					w += r + 1;
					//очистим str1
					explicit_bzero(str1, 0);
					str1 = (char*) realloc(str1, sizeof(char));
					k = 0;
					//очистим arr
					explicit_bzero(arr, 0);
					arr = (char*) realloc(arr, sizeof(char));
					r = 0;
				}
				y = 0;
			}
		}
		else {
			if (r != 0) {
				memcpy(str + i - r - 1, arr, r);
				w += r;
				explicit_bzero(str1, 0);
				str1 = (char*) realloc(str1, sizeof(char));
				k = 0;
				explicit_bzero(arr, 0);
				arr = (char*) realloc(arr, sizeof(char));
				r = 0;
			}
			str[i - 1] = c;
			w++;
			if (c == '\n') countn++;
		}
	}
	if (r != 0) memcpy(str + w, arr, r);
	//покраска
	if (cv != 0) {
		char *pp = strtok(str, "*");
		int a;
		if (str[0] == '*') a = 0;
		else a = 1;
		printf("\n");
		while (pp) {
			a++;
			if (a % 2 != 0) printf("\e[35m%s\e[m", pp);
			else printf("%s", pp);
			pp = strtok(NULL, "*");
		}
	}
	else printf("\n%s", str);
	explicit_bzero(str, 0);
	explicit_bzero(str1, 0);
	explicit_bzero(arr, 0);
	free(str);
	free(str1);
	free(arr);
}
