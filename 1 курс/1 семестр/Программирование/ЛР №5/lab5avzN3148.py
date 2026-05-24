import random
import argparse
from sys import *
import struct

# Генератор MAC-адресов
def MAC_address():
    arr = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F']
    MAC_int = 0
    MAC_str = ''
    while MAC_int < 17:
        MAC_int += 1
        if MAC_int % 3 == 0:
            MAC_str += ':'
        else:
            a = random.randint(0, 15)
            MAC_str += arr[a]
    return MAC_str


if __name__ == "__main__":
    # Создаем парсер аргументов командной строки
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', action='store_true')
    parser.add_argument('-n', type=int, default=random.randint(10, 1000))
    parser.add_argument('file', type=str, nargs='*')

    # Парсим аргументы
    args = parser.parse_args()
    
    # Обрабатываем входные данные
    if len(argv) == 1:
        print('Ошибка: недостаточно аргументов для работы программы!')
        exit(1)
    elif len(argv) == 2 and args.v: # Работа с опцией -v
        print('Анастасия Валерьевна Жук, гр. N3148\nВариант: 3-6')
        exit(0)
    elif (len(argv) == 2 and len(args.file) == 1) or (len(argv) == 4 and argv[1] == '-n' and len(args.file) == 1):
        N = args.n
        
        if N < 0:
            print('Ошибка: введено отрицательное число!')
            exit(6)
        
        MAC_array = [MAC_address() for _ in range(N)] # Список MAC-адресов для списка
        index_list = [] # Массив индексов MAC-адресов: строке MAC_array[i] соответствует индекс index_list[i]
        
        # Каждой строке сопоставляем рандомный индекс
        for _ in range(N):
            a = random.randint(0, N - 1)
            while a in index_list:
                a = random.randint(0, N - 1)
            index_list.append(a)
        
        # Записываем данные в файл
        try:
            with open(args.file[0], 'wb') as f:
                for i in range(N):
                    f.write(struct.pack("H", index_list[i]))
                    f.write(struct.pack("H", 17))
                    f.write(MAC_array[i].encode("utf-8"))
            
        except IOError:
            print('Ошибка: не удалось записать данные в файл!', file=stderr)
            exit(5)

       	# Вывод состояния файла
        print('Строки и их индексы в списке:')
        for i in range(N): 
            print(f'({MAC_array[i]}; {index_list[i]})', end=' ')
        print('\n')
        exit(0)
	
    elif len(argv) > 2 and argv[1] == '-v':
        print('Ошибка: лишние аргументы у опции \'-v\'!')
        exit(2)
    elif len(argv) == 3 and argv[1] == '-n':
        print('Ошибка: не задано имя файла!')
        exit(3)
    else:
        print('Ошибка: при вводе данных обнаружены лишние аргументы!')
        exit(4)


