from collections import deque
import re


class FormatError(Exception):
    pass


class UndoError(Exception):
    pass


class RedoError(Exception):
    pass


class MyDeque(deque):
    @staticmethod
    def _email(email): #проверка на адрес электронной почты
        pattern = re.compile(r"^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$")
        return bool(pattern.match(email))

    def Type_check(self, value):
        if not isinstance(value, str):
            raise TypeError("Ошибка: добавляемый элемент не строка!")
        if not self._email(value):
            raise FormatError("Ошибка: добавляемая строка не адрес электронной почты!")
        return None

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        for i in self:
            self.Type_check(i)
        self._history = []
        self._redo_history = []

    def append(self, item):
        #для корректности работы программы при использовании undo() и redo()
        if self != self._history and self != self._redo_history:
            self.Type_check(item)
            self._history.append(deque(self))
            self._redo_history.clear()
            return super().append(item)

    def appendleft(self, item):
        self.Type_check(item)
        self._history.append(deque(self))
        self._redo_history.clear()
        return super().appendleft(item)

    def pop(self):
        #если нечего удалять, то пропускаем этот шаг (применение pop) без исключений
        if not self:
            return None
        self._history.append(deque(self))
        self._redo_history.clear()
        return super().pop()

    def popleft(self):
        #если нечего удалять, то пропускаем этот шаг (применение popleft) без исключений
        if not self:
            return None
        self._history.append(deque(self))
        self._redo_history.clear()
        return super().popleft()

    def extend(self, item):
        #проверка добавляемых элементов
        for i in item:
            self.Type_check(i)
        self._history.append(deque(self))
        self._redo_history.clear()
        return super().extend(item)

    def extendleft(self, item):
        # проверка добавляемых элементов
        for i in item:
            self.Type_check(i)
        self._history.append(deque(self))
        self._redo_history.clear()
        return super().extend(item)

    def insert(self, index, item):
        self.Type_check(item)
        print(len(self))
        #индекс должен быть числом
        if isinstance(index, int):
            self._history.append(deque(self))
            self._redo_history.clear()
            return super().insert(index, item)

    def remove(self, item):
        self.Type_check(item)
        if item in self:
            self._history.append(deque(self))
            self._redo_history.clear()
            return super().remove(item)

    def clear(self):
        self._history.append(deque(self))
        self._redo_history.clear()
        return super().clear()

    def undo(self):
        if not self._history:
            raise UndoError("Ошибка: нет шагов для undo!")
        self._redo_history.append(deque(self)) #фиксируем текущее состояние
        super().clear()
        super().extend(self._history[-1]) #делаем шаг назад
        self._history = self._history[:-1]

    def redo(self):
        if not self._redo_history:
            raise RedoError("Ошибка: нет шагов для redo!")
        self._history.append(deque(self)) #фиксируем текущее состояние
        super().clear()
        super().extend(self._redo_history[-1]) #возврат
        self._redo_history = self._redo_history[:-1]
