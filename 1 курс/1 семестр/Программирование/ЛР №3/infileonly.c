#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//обрабатывает только целые последовательности (в файле)
void infileonly(char *txt1, char *txt2, int cv, int bv, int ev) {
	FILE *f = fopen(txt1, "r");
	//str - вводимая строка, str1 - вспомогательная строка, которая будет хранить в себе нужную нам подстроку (или что-то похожее на неё)
	char *str = NULL;
	char *str1 = NULL;
	str = (char*) malloc(sizeof(char));
	str1 = (char*) malloc(sizeof(char));
	//для считывания символов
	int t;
	char c;
	int i = 0, k = 0; //i - текущая длина str, k - текущая длина str1
	int y = 0, g = 0; //счетчики
	int countn = 1; //счетчик переносов строки, чтобы следить за номером строки
	while ((t = fgetc(f)) != EOF) {
		//обработка переноса строки и других символов
		if (t == 13) continue;
		else if (t == 10) c = '\n';
		else c = (char) t;
		i++;
		str = (char*) realloc(str, i * sizeof(char)); //увеличим строку
		if ((bv == 1 || countn >= bv) && (ev == 0 || ev >= countn)) { //обработка конкретных строк (ограничения b и e)
			if (isdigit(c) || c == '-') { //будем здесь собирать строки с числами
				k++;
				str1 = (char*) realloc(str1, k * sizeof(char)); //увеличим строку
				str1[k - 1] = c;
				if (c == '-') y++; //счетчик символов '-'
			}
			else {
				if (c == '\n') countn++;
				if (y == 4 && k == 17 && (c == ' ' || c == '\n')) { //проверка, что строка str1 вида ххх-х-ххх-ххххх-х
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
							str[i - 2 - k - 1] = '*';
							memcpy(str + i - 2 - k, str1, k);
							str[i - 2] = '*';
							str[i - 1] = c;
							//очистим str1
							explicit_bzero(str1, 0);
							str1 = (char*) realloc(str1, sizeof(char));
							k = 0;
						}
					} 
					if (f == 0) {
						//записываем в str то, что лежит в str1 (так как так лежит то, что нам не подошло)
						memcpy(str + i - k - 1, str1, k);
						str[i - 1] = c;
						//очистим str1
						explicit_bzero(str1, 0);
						str1 = (char*) realloc(str1, sizeof(char));
						k = 0;
					}
				}
				else { //записываем в str то, что лежит в str1 (так как так лежит то, что нам не подошло)
					memcpy(str + i - k - 1, str1, k);
					str[i - 1] = c;
					//очистим str1
					explicit_bzero(str1, 0);
					str1 = (char*) realloc(str1, sizeof(char));
					k = 0;
				}
				y = 0;
			}
		}
		else {
			str[i - 1] = c;
			if (c == '\n') countn++;
		}
	}
	//запись строки в файл
	if (strlen(txt2) != 0) {
		FILE *f1 = fopen(txt2, "w");
		//покраска
		if (cv != 0) {
			char *pp = strtok(str, "*");
			int a;
			if (str[0] == '*') a = 0;
			else a = 1;
			while (pp) {
				a++;
				if (a % 2 != 0) fprintf(f1, "\e[35m%s\e[m", pp);
				else fprintf(f1, "%s", pp);
				pp = strtok(NULL, "*");
			}
		}
		else fputs(str, f1);
		fclose(f1);		
	}
	else {
		//покраска
		if (cv != 0) {
			char *pp = strtok(str, "*");
			int a;
			if (str[0] == '*') a = 0;
			else a = 1;
			while (pp) {
				a++;
				if (a % 2 != 0) printf("\e[35m%s\e[m", pp);
				else printf("%s", pp);
				pp = strtok(NULL, "*");
			}
		}
		else printf("%s", str);
	}
	explicit_bzero(str, 0);
	explicit_bzero(str1, 0);
	free(str);
	free(str1);
	fclose(f);
}
