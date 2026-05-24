#include <stdio.h>
#include <stdlib.h> //Здесь содержится функция rand()
#include <time.h> //Чтобы использовать функцию time()
#include <string.h> //Для NULL

void perevod (unsigned short n) { // Функция вывода двоичного числа побайтно 
	for (int i = sizeof(n) * 8; i > 0; i--) { 
		if (i % 8 == 0 && i != sizeof(n) * 8) 
			printf(" "); 
		printf("%d", n & (1 << (i - 1)) ? 1 : 0);
	}
        printf("\n");
}

int main(int argc, char *argv[]) {
	// Проверка запуска с переменной среды, включающей отладочный вывод.
	// Пример запуска с установкой переменной LAB1DEBUG в 1:
	// $ LAB1DEBUG=1 ./lab1avsN3148 123
	char *DEBUG = getenv("LAB1DEBUG");
	if (DEBUG) {
	    fprintf(stderr, "Включен вывод отладочных сообщений\n");
	}
    
	/*
	if (argc != 2) {
	    fprintf(stderr, "Usage: %s [число]\n", argv[0]);
	    return EXIT_FAILURE;
	}
	*/
    
    	unsigned short n;
	long long k;
	char errno;
	int flag = 0;
	
	srand(time(NULL)); //Для генерации рандомных чисел
	
	//Обработка ошибок: проверка на корректность входных данных
	if (argc == 1) {
		n = rand();
		printf("%hu\n", n);
		flag = 1;

	} 
	else if (argc == 2) {
		
		if (sscanf(argv[1], "%lld %c", &k, &errno) != 1) 
			printf("Ошибка: \'%s\' не является числом.\n", argv[1]);
			
		else if (k < 0 || k > 65535) 
			printf("Ошибка: значение выходит за допустимый диапазон.\n");

		else {
			n = k;
			flag = 1;
		}
	}
	
	if (flag) { //Если входные данные корректны, можно переходить к выполнению программы
		perevod(n);
		
		int t, p;
		t = sizeof(n);
		unsigned short n1, n2, n3;
		unsigned short ans = 0; //Здесь будет лежать ответ
		
		unsigned short mask1 = 0b11000000, mask2 = 0b11110000, mask3 = 0b1111, mask4 = 0b11110000;
		for (int i = 1; i <= t; i++) {
		
			p = n & mask1; 
			mask1 <<= 8; //Обновили mask1
			p >>= 6 + (i - 1) * 8; //Нашли число, на которое нужно двигать младшую тетраду i-ого байта
			
			n1 = n & mask3; //Находим младшую тетраду i-ого байта
			n1 <<= p; //Делаем сдвиг младшей тетрады
			n2 = n1 & mask2; 
			n1 = n1 & mask3;
			n2 >>= 4;
			n3 = n1 | n2; //Получили новую тетраду: соединяем две части новой младшей тетрады
			
			mask2 <<= 8; //Обновили mask2
			mask3 <<= 8; //Обновили mask3
			
			ans |= n3; //Записали в ответ новую младшую тетраду i-ого байта
			ans |= n & mask4; //Записали в ответ старую старшую тетраду i-ого байта
			mask4 <<= 8; //Обновили mask4
			
			
		}
		n = ans;
		perevod(n);
	}
    
    return EXIT_SUCCESS;
}
