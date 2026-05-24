import os
import sys
import hashlib
import fnmatch
import shutil
from pathlib import Path
import logging
import ctypes
import win32security
import ntsecuritycon
import win32api
import win32con
import time
import threading
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler


class FileGuardEventHandler(FileSystemEventHandler):
    def __init__(self, file_guard):
        self.file_guard = file_guard

    def on_created(self, event):
        # Блокируем создание файлов по маскам
        if not event.is_directory and self.file_guard.matches_pattern(event.src_path):
            self.file_guard.block_file_operation(event.src_path, "создание")

    def on_deleted(self, event):
        # Блокируем удаление файлов по маскам
        if not event.is_directory and self.file_guard.matches_pattern(event.src_path):
            self.file_guard.block_file_operation(event.src_path, "удаление")

    def on_moved(self, event):
        # Блокируем переименование и перемещение файлов по маскам
        if not event.is_directory:
            # Блокируем если исходный файл под маской
            if self.file_guard.matches_pattern(event.src_path):
                self.file_guard.block_file_operation(event.src_path, "переименование")
            # Блокируем если целевой файл под маской (при копировании)
            elif self.file_guard.matches_pattern(event.dest_path):
                self.file_guard.block_file_copy(event.src_path, event.dest_path)

    def on_modified(self, event):
        # Блокируем изменение защищенных файлов
        if not event.is_directory and self.file_guard.matches_pattern(event.src_path):
            self.file_guard.block_file_operation(event.src_path, "изменение")


class SimpleFileGuard:
    def __init__(self):
        # Инициализация основных переменных
        self.current_dir = Path.cwd()  # Текущая рабочая директория
        self.template_file = self.current_dir / "template.tbl"  # Файл с настройками
        self.is_protected = False  # Флаг активности защиты
        self.patterns = []  # Список масок для защиты
        self.correct_password_hash = None  # Сохраняем хеш пароля ДО блокировки файла
        self.observer = None  # Объект для мониторинга файловой системы

        # Настройка системы логирования
        logging.basicConfig(
            filename=self.current_dir / 'file_guard.log',  # Файл для логов
            level=logging.INFO,  # Уровень детализации
            format='%(asctime)s - %(levelname)s - %(message)s'  # Формат записи
        )

    def is_protection_active(self):
        # Провека активности защиты (файл template.tbl заблокирован или нет)
        try:
            # Пытаемся прочитать файл - если не получается, значит защита активна
            with open(self.template_file, 'r') as f:
                f.read(1)  # Пытаемся прочитать хотя бы 1 символ
            self.is_protected = False
            return False  # Файл читается - защита не активна
        except PermissionError:
            self.is_protected = True
            return True  # Нет доступа - защита активна
        except Exception:
            self.is_protected = False
            return False  # Другие ошибки - считаем что защита не активна

    def check_prerequisites_smart(self):
        # Умная проверка условий с учетом состояния защиты

        # 1. Проверка прав администратора
        if not self.is_admin():
            print("Ошибка: Запустите программу от имени администратора!")
            return False

        # 2. Проверка существования файла template.tbl
        if not self.template_file.exists():
            print("Ошибка: Файл template.tbl не найден в текущей папке!")
            print("Создайте файл template.tbl с следующим форматом:")
            print(" - Первая строка: хеш пароля")
            print(" - Последующие строки: маски файлов (по одной на строку)")
            return False

        # 3. Если файл заблокирован (защита активна) - пропускаем проверку формата
        if self.is_protection_active():
            print("Обнаружена активная защита - файл template.tbl заблокирован")
            return True  # Защита активна, можно выполнять stop/status

        # 4. Если файл не заблокирован - проверяем формат
        if not self.validate_template_format_and_save_hash():
            print("Ошибка: Неверный формат файла template.tbl!")
            return False

        return True

    def is_admin(self):
        # Проверка, запущена ли программа с правами администратора Windows
        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False

    def validate_template_format_and_save_hash(self):
        # Проверка формата template.tbl и сохранение хеша пароля в память
        try:
            with open(self.template_file, 'r') as f:
                # Читаем все непустые строки файла
                lines = [line.strip() for line in f.readlines() if line.strip()]

            # Проверка минимального количества строк (пароль + хотя бы одна маска)
            if len(lines) < 2:
                print("Ошибка: В template.tbl должен быть пароль и хотя бы одна маска")
                return False

            # Проверка формата хеша пароля (должен быть SHA-256 - 64 hex символа)
            password_hash = lines[0]
            if len(password_hash) != 64 or not all(c in '0123456789abcdef' for c in password_hash):
                print("Ошибка: Первая строка должна быть хешем SHA-256 (64 символа)")
                return False

            # Сохраняем хеш пароля в память ДО блокировки файла
            self.correct_password_hash = password_hash
            print("Формат template.tbl корректен, хеш пароля сохранен")
            return True

        except Exception as e:
            print(f"Ошибка чтения template.tbl: {e}")
            return False

    def verify_password(self, password):
        # Проверка введенного пароля с сохраненным хешем (не читаем файл!)
        if self.correct_password_hash is None:
            print("Ошибка: Хеш пароля не был сохранен")
            return False

        # Хешируем введенный пароль и сравниваем с сохраненным хешем
        input_hash = hashlib.sha256(password.encode()).hexdigest()
        return input_hash == self.correct_password_hash

    def load_patterns(self):
        # Загрузка масок защищаемых файлов из template.tbl
        # Этот метод вызывается только ДО блокировки файла
        try:
            with open(self.template_file, 'r') as f:
                lines = f.readlines()
                # Пропускаем первую строку (пароль), остальные - маски файлов
                self.patterns = [line.strip() for line in lines[1:] if line.strip()]

            print(f"Загружены маски: {self.patterns}")
            logging.info(f"Загружены маски: {self.patterns}")

        except Exception as e:
            print(f"Ошибка загрузки масок: {e}")
            logging.error(f"Ошибка загрузки масок: {e}")
            self.patterns = []

    def matches_pattern(self, filename):
        # Проверка, соответствует ли имя файла любой из масок
        filename = Path(filename).name  # Берем только имя файла без пути
        # Используем fnmatch для сравнения с каждой маской
        return any(fnmatch.fnmatch(filename, pattern) for pattern in self.patterns)

    def is_template_file(self, file_path):
        # Проверка, является ли файл template.tbl
        return Path(file_path).name.lower() == "template.tbl"

    def block_file_operation(self, file_path, operation):
        # Блокировка файловой операции
        try:
            # Защищаем файл от изменений
            self.protect_single_file(file_path)

            # Логируем попытку нарушения
            logging.warning(f"Заблокирована попытка {operation} файла: {file_path}")
            print(f"Заблокирована попытка {operation} файла: {Path(file_path).name}")

        except Exception as e:
            logging.error(f"Ошибка блокировки операции {operation} для {file_path}: {e}")

    def block_file_copy(self, src_path, dest_path):
        # Блокировка копирования файлов
        try:
            # Определяем тип копирования
            src_name = Path(src_path).name
            dest_name = Path(dest_path).name

            # Блокируем копирование template.tbl
            if self.is_template_file(src_path):
                self.delete_copied_file(dest_path)
                logging.warning(f"Заблокировано копирование template.tbl в: {dest_path}")
                print(f"Заблокировано копирование template.tbl")
                return

            # Блокируем копирование файлов по маскам
            if self.matches_pattern(src_path):
                self.delete_copied_file(dest_path)
                logging.warning(f"Заблокировано копирование защищенного файла {src_name} в: {dest_name}")
                print(f"Заблокировано копирование защищенного файла: {src_name}")
                return

            # Блокируем создание файлов по маскам в целевой папке
            if self.matches_pattern(dest_path):
                self.delete_copied_file(dest_path)
                logging.warning(f"Заблокировано создание защищенного файла через копирование: {dest_name}")
                print(f"Заблокировано создание защищенного файла: {dest_name}")

        except Exception as e:
            logging.error(f"Ошибка блокировки копирования из {src_path} в {dest_path}: {e}")

    def delete_copied_file(self, file_path):
        # Удаление скопированного файла с многократными попытками
        max_attempts = 10
        for attempt in range(max_attempts):
            try:
                if os.path.exists(file_path):
                    # Снимаем защиту если нужно
                    try:
                        self.unprotect_single_file(file_path)
                    except:
                        pass
                    # Удаляем файл
                    os.remove(file_path)
                    return True
                else:
                    return True  # Файл уже удален
            except Exception as e:
                if attempt < max_attempts - 1:
                    time.sleep(0.1)  # Короткая пауза перед повторной попыткой
                else:
                    logging.error(f"Не удалось удалить скопированный файл {file_path}: {e}")
        return False

    def protect_single_file(self, file_path):
        # Защита одного файла от изменений и удаления
        try:
            sd = win32security.GetFileSecurity(
                str(file_path),
                win32security.DACL_SECURITY_INFORMATION
            )

            dacl = win32security.ACL()
            everyone_sid = win32security.ConvertStringSidToSid("S-1-1-0")

            # Запрещаем опасные операции
            dacl.AddAccessDeniedAce(
                win32security.ACL_REVISION,
                ntsecuritycon.DELETE |
                ntsecuritycon.WRITE_DAC |
                ntsecuritycon.WRITE_OWNER |
                ntsecuritycon.FILE_WRITE_DATA |
                ntsecuritycon.FILE_WRITE_ATTRIBUTES |
                ntsecuritycon.FILE_APPEND_DATA,
                everyone_sid
            )

            # Разрешаем только чтение
            dacl.AddAccessAllowedAce(
                win32security.ACL_REVISION,
                ntsecuritycon.FILE_READ_DATA |
                ntsecuritycon.FILE_READ_ATTRIBUTES |
                ntsecuritycon.FILE_READ_EA |
                ntsecuritycon.READ_CONTROL,
                everyone_sid
            )

            sd.SetSecurityDescriptorDacl(1, dacl, 0)
            win32security.SetFileSecurity(
                str(file_path),
                win32security.DACL_SECURITY_INFORMATION,
                sd
            )

        except Exception as e:
            logging.error(f"Ошибка защиты файла {file_path}: {e}")

    def unprotect_single_file(self, file_path):
        # Снятие защиты с одного файла
        try:
            sd = win32security.GetFileSecurity(
                str(file_path),
                win32security.DACL_SECURITY_INFORMATION
            )

            dacl = win32security.ACL()
            everyone_sid = win32security.ConvertStringSidToSid("S-1-1-0")

            dacl.AddAccessAllowedAce(
                win32security.ACL_REVISION,
                win32con.GENERIC_ALL,
                everyone_sid
            )

            sd.SetSecurityDescriptorDacl(1, dacl, 0)
            win32security.SetFileSecurity(
                str(file_path),
                win32security.DACL_SECURITY_INFORMATION,
                sd
            )

        except Exception as e:
            logging.error(f"Ошибка разблокировки файла {file_path}: {e}")

    def protect_template_file(self):
        # Полная защита файла template.tbl от удаления, просмотра и модификации
        try:
            # Получаем дескриптор безопасности файла
            sd = win32security.GetFileSecurity(
                str(self.template_file),
                win32security.DACL_SECURITY_INFORMATION
            )

            # Создаем пустой DACL - это означает полный запрет доступа для всех
            dacl = win32security.ACL()
            sd.SetSecurityDescriptorDacl(1, dacl, 0)

            # Применяем новые права доступа к файлу
            win32security.SetFileSecurity(
                str(self.template_file),
                win32security.DACL_SECURITY_INFORMATION,
                sd
            )

            print("Файл template.tbl полностью защищен (все операции запрещены)")
            logging.info("Файл template.tbl полностью защищен")

        except Exception as e:
            print(f"Ошибка защиты template.tbl: {e}")
            logging.error(f"Ошибка защиты template.tbl: {e}")

    def unprotect_template_file(self):
        # Полное снятие защиты с файла template.tbl
        try:
            sd = win32security.GetFileSecurity(
                str(self.template_file),
                win32security.DACL_SECURITY_INFORMATION
            )

            # Создаем DACL с полными правами для всех пользователей
            dacl = win32security.ACL()
            everyone_sid = win32security.ConvertStringSidToSid("S-1-1-0")

            dacl.AddAccessAllowedAce(
                win32security.ACL_REVISION,
                win32con.GENERIC_ALL,  # Все возможные права
                everyone_sid
            )

            sd.SetSecurityDescriptorDacl(1, dacl, 0)
            win32security.SetFileSecurity(
                str(self.template_file),
                win32security.DACL_SECURITY_INFORMATION,
                sd
            )

            print("Защита template.tbl полностью снята")
            logging.info("Защита template.tbl полностью снята")

        except Exception as e:
            print(f"Ошибка снятия защиты template.tbl: {e}")
            logging.error(f"Ошибка снятия защиты template.tbl: {e}")

    def apply_directory_protection(self):
        # Защита существующих файлов по маскам и запуск мониторинга
        try:
            # 1. Защищаем существующие файлы, которые подходят под маски
            existing_files = list(self.current_dir.iterdir())
            protected_count = 0

            for file_path in existing_files:
                if file_path.is_file() and (self.matches_pattern(file_path.name) or self.is_template_file(file_path)):
                    self.protect_single_file(file_path)
                    protected_count += 1

            # 2. Запускаем мониторинг новых операций
            self.start_file_monitoring()

            self.is_protected = True
            print(f"Защита активирована. Защищено файлов: {protected_count}")
            print(f"Мониторинг операций с файлами: {self.patterns}")
            print("Также защищен от копирования: template.tbl")
            logging.info(f"Защита активирована. Защищено файлов: {protected_count}")

        except Exception as e:
            print(f"Ошибка применения защиты: {e}")
            logging.error(f"Ошибка применения защиты: {e}")

    def remove_directory_protection(self):
        # Снятие защиты со всех файлов и остановка мониторинга
        try:
            # 1. Останавливаем мониторинг
            self.stop_file_monitoring()

            # 2. Снимаем защиту со всех файлов в папке
            existing_files = list(self.current_dir.iterdir())
            unprotected_count = 0

            for file_path in existing_files:
                if file_path.is_file():
                    try:
                        self.unprotect_single_file(file_path)
                        unprotected_count += 1
                    except:
                        pass  # Игнорируем ошибки для уже разблокированных файлов

            self.is_protected = False
            print(f"Защита отключена. Разблокировано файлов: {unprotected_count}")
            logging.info(f"Защита отключена. Разблокировано файлов: {unprotected_count}")

        except Exception as e:
            print(f"Ошибка снятия защиты: {e}")
            logging.error(f"Ошибка снятия защиты: {e}")

    def start_file_monitoring(self):
        # Запуск мониторинга файловых операций
        try:
            event_handler = FileGuardEventHandler(self)
            self.observer = Observer()
            self.observer.schedule(event_handler, str(self.current_dir), recursive=False)
            self.observer.start()
            logging.info("Мониторинг файловых операций запущен")
        except Exception as e:
            print(f"Ошибка запуска мониторинга: {e}")
            logging.error(f"Ошибка запуска мониторинга: {e}")

    def stop_file_monitoring(self):
        # Остановка мониторинга файловых операций
        try:
            if self.observer:
                self.observer.stop()
                self.observer.join()
                self.observer = None
                logging.info("Мониторинг файловых операций остановлен")
        except Exception as e:
            print(f"Ошибка остановки мониторинга: {e}")
            logging.error(f"Ошибка остановки мониторинга: {e}")

    def start_protection(self):
        # Основная функция включения защиты

        # Даем 3 попытки ввода пароля
        for attempt in range(3):
            password = input("Введите пароль для включения защиты: ")
            if self.verify_password(password):
                # При успешной аутентификации:
                self.load_patterns()  # Загружаем маски файлов (до блокировки!)
                self.protect_template_file()  # Полностью защищаем файл настроек
                self.apply_directory_protection()  # Включаем защиту папки
                return True
            else:
                remaining = 2 - attempt
                print(f"Неверный пароль. Осталось попыток: {remaining}")

        print("Превышено количество попыток. Защита не активирована.")
        return False

    def stop_protection(self):
        # Основная функция отключения защиты

        # Разблокируем файл для проверки пароля
        self.unprotect_template_file()

        if not self.validate_template_format_and_save_hash():
            print("Ошибка: Неверный формат файла template.tbl!")
            return False

        # Включаем защиту файла
        self.protect_template_file()

        # Даем 3 попытки ввода пароля
        for attempt in range(3):
            password = input("Введите пароль для отключения защиты: ")
            if self.verify_password(password):
                # При успешной аутентификации:
                self.remove_directory_protection()  # Снимаем защиту с папки
                self.unprotect_template_file()  # Полностью разблокируем файл настроек
                return True
            else:
                remaining = 2 - attempt
                print(f"Неверный пароль. Осталось попыток: {remaining}")

        print("Превышено количество попыток. Защита остается активной.")
        return False

    def show_status(self):
        # Отображение текущего статуса защиты
        status = "ВКЛЮЧЕНА" if self.is_protected else "ВЫКЛЮЧЕНА"
        print(f"Статус защиты: {status}")
        print(f"Текущая папка: {self.current_dir}")


def main():
    # Главная функция программы
    guard = SimpleFileGuard()

    # Проверка всех необходимых условий перед работой
    flag = guard.check_prerequisites_smart()
    if not flag:
        return

    # Обработка аргументов командной строки
    if len(sys.argv) > 1:
        if sys.argv[1] == "start":
            if not guard.is_protection_active():
                guard.start_protection()
        elif sys.argv[1] == "stop":
            guard.stop_protection()
        elif sys.argv[1] == "status":
            guard.show_status()
        else:
            print("Неизвестная команда")
            print("Использование: python file_guard.py [start|stop|status]")
    else:
        # Вывод справки если аргументы не указаны
        print("Использование: python file_guard.py [start|stop|status]")
        print("Обязательно запускайте от имени администратора!")


if __name__ == "__main__":
    # Точка входа в программу
    main()