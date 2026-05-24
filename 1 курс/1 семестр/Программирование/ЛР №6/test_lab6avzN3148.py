from lab6avzN3148 import MyDeque, FormatError, UndoError, RedoError


def test_1():
    #Различные варианты добавления в очередь адресов
    MD = MyDeque(['kate@yandex.ru'])
    MD.append('nastenkazhuk05@mail.ru')
    MD.insert(1, 'dom@gmail.com')
    MD.appendleft('book2005@mail.ru')
    assert MD == MyDeque(['book2005@mail.ru', 'kate@yandex.ru', 'dom@gmail.com', 'nastenkazhuk05@mail.ru'])


def test_2():
    #Неверный ввод адреса - неверный формат
    MD = MyDeque([])
    try:
        MD.append('dfkfghiughirt')
    except FormatError:
        err = 1
    assert err == 1


def test_3():
    # Неверный ввод адреса - неправильный тип
    MD = MyDeque([])
    try:
        MD.append(521)
    except TypeError:
        err = 1
    assert err == 1


def test_4():
    #Всевозможное удаление элементов
    MD = MyDeque(['book2005@mail.ru', 'nastenkazhuk05@mail.ru', 'kate@yandex.ru', 'nastenkazhuk05@mail.ru', 'dom@gmail.com'])
    MD.pop()
    MD.popleft()
    MD.remove('nastenkazhuk05@mail.ru')
    assert MD == MyDeque(['kate@yandex.ru', 'nastenkazhuk05@mail.ru'])

def test_5():
    #Проверим в действии функции, которые добавляют сразу несколько элементов
    MD = MyDeque([])
    MD.extend(('book2005@mail.ru', 'nastenkazhuk05@mail.ru'))
    MD.extendleft(['kate@yandex.ru', 'dom@gmail.com'])
    assert MD == MyDeque(['book2005@mail.ru', 'nastenkazhuk05@mail.ru', 'kate@yandex.ru', 'dom@gmail.com'])


def test_6():
    # Проверим в действии функции, которые добавляют сразу несколько элементов, среди которых есть не адрес
    MD = MyDeque([])
    try:
        MD.extend(('book2005@mail.ru', 'nastenkazhuk05mail.ru'))
    except:
        err = 1
    assert err == 1


def test_7():
    #Проверим очистку очереди
    MD = MyDeque(['book2005@mail.ru', 'nastenkazhuk05@mail.ru', 'kate@yandex.ru', 'nastenkazhuk05@mail.ru', 'dom@gmail.com'])
    MD.clear()
    assert MD == MyDeque([])


def test_8():
    #Проверка работы методов undo() и redo() - хороший случай, без исключений
    MD = MyDeque(['nastenkazhuk05@mail.ru'])
    try:
        MD.append('kate@yandex.ru')
        MD.append('dom@gmail.com')
        MD.undo()
        MD.undo()
        MD.redo()
        MD.redo()
    except:
        err = 1
    assert MD == MyDeque(['nastenkazhuk05@mail.ru', 'kate@yandex.ru', 'dom@gmail.com'])


def test_9():
    #Проверка работы методов undo() и redo() - ошибка при запуске undo()
    MD = MyDeque(['nastenkazhuk05@mail.ru'])
    try:
        MD.append('kate@yandex.ru')
        MD.append('dom@gmail.com')
        MD.undo()
        MD.undo()
        MD.undo()
    except UndoError:
        err = 1
    assert err == 1


def test_10():
    #Проверка работы методов undo() и redo() - ошибка при запуске redo()
    MD = MyDeque(['nastenkazhuk05@mail.ru'])
    try:
        MD.append('kate@yandex.ru')
        MD.append('dom@gmail.com')
        MD.redo()
    except RedoError:
        err = 1
    assert err == 1


def test():
    MD = MyDeque(['dom@gmail.com'])
    MD.insert(100, 'sdf@d.h')
    assert MD == MyDeque(['dom@gmail.com', 'sdf@d.h'])


