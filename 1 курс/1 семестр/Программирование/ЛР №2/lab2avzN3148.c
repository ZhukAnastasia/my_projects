#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h> //Чтобы использовать функцию time()
#include <string.h> //Для NULL
#include <ctype.h> //Для isdigit

short* smeshenie(short arr[], int i1, int j1, int m, int k, int v) {
	short new[k * k - (k - 2) * (k - 2) + v];
	//создаем массив, который будет представлять из себя кольцо со сдвигами
        int i = i1, j = j1;
    	//заполняем его сдвинутыми элементами
        for (int p = v; p < k * k - (k - 2) * (k - 2) + v; p++) {
    
    	if (p < k * k - (k - 2) * (k - 2)) new[p] = arr[i * m + j];
    	else new[p - (k * k - (k - 2) * (k - 2))] = arr[i * m + j];
    	
    	if (i == i1 && j < j1 + k - 1) j++;
    	else if (j == j1 + k - 1 && i < i1 + k - 1) i++;
	else if (i == i1 + k - 1 && j > j1) j--;
	else if (j == j1 && i > i1) i--;
    }
    
    
    //а теперь этот новый массив нужно вставить на место старого кольца
    i = i1; 
    j = j1;
    for (int p = 0; p < k * k - (k - 2) * (k - 2); p++) {
    	arr[i * m + j] = new[p];
    	if (i == i1 && j < j1 + k - 1) j++;
    	else if (j == j1 + k - 1 && i < i1 + k - 1) i++;
	else if (i == i1 + k - 1 && j > j1) j--;
	else if (j == j1 && i > i1) i--;
    }
    return arr; //вернет массив со сдвинутым кольцом
    
}

int main(int argc, char *argv[]) {
    // Проверка запуска с переменной среды, включающей отладочный вывод.
    // Пример запуска с установкой переменной LAB2DEBUG в 1:
    // $ LAB2DEBUG=1 ./lab2avzN3148 5 5
    char *DEBUG = getenv("LAB2DEBUG");
    if (DEBUG) {
        fprintf(stderr, "Включен вывод отладочных сообщений\n");
    }

    int n = 0, m = 0;
	srand(time(NULL)); //Для генерации рандомных чисел
	//считываем n и m, если они есть
	if (argc < 3) {
		printf("Ошибка: недостаточно аргументов.\n");
		return 0;
	}
	else if (argc == 3) {
		if (strcmp(argv[1], "-m") == 0) {
			printf("Ошибка: отсутствует обязательный аргумент.\n");
			return 0;
		}
		else if (sscanf(argv[1], "%d", &n) == 0) {
			printf("Ошибка: \'%s\' не является числом.\n", argv[1]);
			return 0;
		}
		else if (sscanf(argv[2], "%d", &m) == 0) {
			printf("Ошибка: \'%s\' не является числом.\n", argv[2]);
			return 0;
		}
	} 
	else if (argc == 4) {
		if (strcmp(argv[1], "-m") != 0) {
			printf("Ошибка: опция '%s' не поддерживается.\n", argv[1]);
			return 0;
		}
		else if (sscanf(argv[2], "%d", &n) == 0) {
			printf("Ошибка: \'%s\' не является числом.\n", argv[2]);
			return 0;
		}
		else if (sscanf(argv[3], "%d", &m) == 0) {
			printf("Ошибка: \'%s\' не является числом.\n", argv[3]);
			return 0;
		}
	}
	//если корректно введена размерность матрицы, то можно ее создавать
	if (n != 0 && m != 0) {
	 	//рандомные значения в метрице
		short arr[n * m];
		if (argc == 3) {
			printf("Исходная матрица:\n");
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < m; j++) {
					short w = rand(); 
					arr[i * m + j] = w;
					printf("%hd ", w);
				}
				printf("\n");
			}
		
		}
		//пользователь сам вводит значения
		else if (argc == 4) {
			printf("Введите строки матрицы:\n");
			int i = 0, j = 0; //координаты вводимого значения
			int g = 0; //шаг
			short s; //считанное значение
			long long t;
			int p; //сдвиг
			char str[7 * m + 1]; //6 * m + (m - 1) + m + 1
			while (i < n) {
				//считываем строку, вытаскиваем из нее подходяцие числа и обрабатываем ошибки
				fgets(str, 7 * m, stdin);
				if (str[0] == ' ' && str[0] != '-') {
					printf("Ошибка: строка не должна начинаться с пробела!\n");
					return 0;
				}
				if (str[0] == '\n') {
					printf("Ошибка: недостаточно переменных!\n");
					return 0;
				}
				while (str[g] != '\0' || g < 7 * m + 1) {
					if (isdigit(str[g]) || str[g] == '-') {
						sscanf(str + g, "%lld%n", &t, &p);
						if (t > 32767 || t < -32768) {
							printf("Ошибка: тип данных некорректный!\n");
							return 0;
						}
						else s = t;
						if (str[g + p] != ' ' && str[g + p] != '\n' && str[g + p] != '-') {
							printf("Ошибка: некорректный ввод данных - присутствуют не только цифры!\n");
							return 0;
						}
						arr[i * m + j] = s;	
						j++;
						if (j > m) {
							printf("Ошибка: много переменных!\n");
							return 0;
						}
						if (str[g + p] == '\n') break;
						g += p + 1;
					} 
					else if (str[g] == ' '){
						printf("Ошибка: некорректный ввод данных - слишком много пробелов!\n");
						return 0;
					}
					else {
						printf("Ошибка: некорректный ввод данных - присутствуют не только цифры!\n");
						return 0;
					}
				}
				i++;
				if (j < m) {
					printf("Ошибка: недостаточно переменных!\n");
					return 0;
				}
				j = 0;
				p = 0;
				g = 0;
			}
			printf("Исходная матрица:\n");
		
			for (int i = 0; i < n; i++) {
	    			for (int j = 0; j < m; j++) {
	    				printf("%d ", arr[i * m + j]);
	    			} 
	    			printf("\n");
	    		}
		}
		
		printf("Введите порядок кольца и количество сдвигов:\n");
		int k, v;
		scanf("%d %d", &k, &v);

		//массив ans нужен для того, чтобы считать характуристики колец
		int l = 65536 * (k * k - (k - 2) * (k - 2)); //такая размернрсть нужна для того, чтобы мы могли обращаться к массиву по суммам, а не бегать по нему и искать схожие значения сумм
	 	int ans[l]; // (power(2, 8 * sizeof(short))) * (k * k - (k - 2) * (k - 2)) = 65536 * (k * k - (k - 2) * (k - 2)) 
	   	for (int i = 0; i < l; i++) 
			ans[i] = -1; //забили -1 для того, чтобы дальнейшая проверка прошла корректно
	    	int flag = 0;
	   	int x1 = 0, x2 = 0; //начальные координаты колец
	    	for (int i = 0; i <= n - k; i++) {
			for (int j = 0; j <= m - k; j++) {
		        	int a = 0;
		        	a += arr[i * m + j] + arr[(i + k - 1) * m + j] + arr[i * m + (j + k - 1)] + arr[(i + k - 1) * m + (j + k - 1)];
		        	for (int b = 1; b < k - 1; b++) {
		                	a += arr[(i + b) * m + j] + arr[i * m + (j + b)] + arr[(i + k - 1 - b) * m + (j + k - 1)] + arr[(i + k - 1) * m + (j + k - 1 - b)];
		        	} //подсчитываем характеристику кольца
		        	if (ans[a + 32768 * (k * k - (k - 2) * (k - 2))] == -1) {
		        		ans[a + 32768 * (k * k - (k - 2) * (k - 2))] = i * m + j;
		        	}
		        	else {
				    	x1 = ans[a + 32768 * (k * k - (k - 2) * (k - 2))]; 
				    	x2 = m * i + j;
				    	flag = 1;
				    	break;
		        	}
			}
			if (flag == 1) break;
	    	}
	    	
	    	if (x1 == x2) { //пара не нашлась
	    		printf("Кольца с одинаковыми характеристиками не найдены.\n");
	    		return 0;
	    	}
	    	
	    	int i1 = x1 / m, j1 = x1 - i1 * m;
	    	int i2 = x2 / m, j2 = x2 - i2 * m;
	    
	    	smeshenie(arr, i1, j1, m, k, v);
	    	smeshenie(arr, i2, j2, m, k, v);
	    	
	    	printf("Результат:\n");
	    	for (int i = 0; i < n; i++) {
	    		for (int j = 0; j < m; j++) {
	    			printf("%d ", arr[i * m + j]);
	    		} 
	    		printf("\n");
	    	}
    	}
    
    return EXIT_SUCCESS;
}
