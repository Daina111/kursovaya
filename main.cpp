#include <iostream>
#include <string>
#include <locale>
#include <windows.h>
#include <stdexcept> // для std::invalid_argument и др.

#include "binary_file.h"
#include "Student.h"
#include "Time.h"

using namespace std;

template<typename T>
void menu(BinaryFile<T>& file, const string& typeName) {
    int choice;

    while (true) {
        cout << "\n=== Менеджер бинарного файла: " << typeName << " ===\n";
        cout << "1. Показать все объекты\n";
        cout << "2. Добавить объект\n";
        cout << "3. Получить объект по индексу\n";
        cout << "4. Обновить объект по индексу\n";
        cout << "5. Удалить объект по индексу\n";
        cout << "6. Отсортировать объекты\n";
        cout << "7. Показать информацию о файле\n";
        cout << "8. Выход\n";
        cout << "Выберите опцию: ";

        if (!(cin >> choice)) {
            cout << "Ошибка ввода, выхожу из меню.\n";
            return;
        }
        cin.ignore();

        try {
            switch (choice) {
            case 1: {
                cout << "\n--- Список объектов ---\n";
                file.displayAll();
                break;
            }
            case 2: {
                T obj;
                cout << "\n--- Добавление нового объекта ---\n";
                cin >> obj;          // здесь могут вылетать invalid_argument / out_of_range
                file.add(obj);
                cout << "Объект успешно добавлен!\n";
                break;
            }
            case 3: {
                int index;
                cout << "\n--- Поиск объекта по индексу ---\n";
                cout << "Введите индекс: ";
                cin >> index;
                cin.ignore();

                T obj;
                file.get(index, obj);   // если индекс неверный — бросит исключение
                cout << "Найден объект с индексом " << index << ": " << obj << "\n";
                break;
            }
            case 4: {
                int index;
                cout << "\n--- Обновление объекта ---\n";
                cout << "Введите индекс для обновления: ";
                cin >> index;
                cin.ignore();

                T obj;
                cout << "Введите новые данные объекта:\n";
                cin >> obj;

                file.update(index, obj);  // тоже может кинуть исключение
                cout << "Объект успешно обновлен!\n";
                break;
            }
            case 5: {
                int index;
                cout << "\n--- Удаление объекта ---\n";
                cout << "Введите индекс для удаления: ";
                cin >> index;
                cin.ignore();

                file.remove(index);       // может кинуть out_of_range
                cout << "Объект успешно удален!\n";
                break;
            }
            case 6: {
                file.sort();
                cout << "Объекты успешно отсортированы!\n";
                break;
            }
            case 7: {
                cout << "\n--- Информация о файле ---\n";
                cout << "Файл содержит " << file.getSize() << " объектов\n";
                break;
            }
            case 8: {
                file.closeFile();
                cout << "Выход из меню.\n";
                return;
            }
            default:
                cout << "Неверная опция!\n";
                break;
            }
        }
        catch (const std::bad_alloc& e) {
            cout << "Ошибка: недостаточно памяти: " << e.what() << "\n";
        }
        catch (const std::out_of_range& e) {
            cout << "Ошибка диапазона: " << e.what() << "\n";
        }
        catch (const std::invalid_argument& e) {
            cout << "Ошибка ввода: " << e.what() << "\n";
        }
        catch (const std::runtime_error& e) {
            cout << "Ошибка выполнения: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            cout << "Неожиданная ошибка: " << e.what() << "\n";
        }
        catch (...) {
            cout << "Произошло неизвестное исключение.\n";
        }
    }
}

int main() {
    try {
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
        setlocale(LC_ALL, "Russian");

        int dataType;

        cout << "=== Система управления бинарными файлами ===\n";
        cout << "Выберите тип данных:\n";
        cout << "1. Целые числа (int)\n";
        cout << "2. Вещественные числа (double)\n";
        cout << "3. Студенты\n";
        cout << "4. Время с напоминанием\n";
        cout << "Введите выбор: ";
        cin >> dataType;
        cin.ignore();

        switch (dataType) {
        case 1: {
            BinaryFile<int> intFile("integers.dat");
            intFile.create();
            intFile.openForWork();
            menu(intFile, "Целые числа");
            break;
        }
        case 2: {
            BinaryFile<double> doubleFile("doubles.dat");
            doubleFile.create();
            doubleFile.openForWork();
            menu(doubleFile, "Вещественные числа");
            break;
        }
        case 3: {
            BinaryFile<Student> studentFile("students.dat");
            studentFile.create();
            studentFile.openForWork();
            menu(studentFile, "Студенты");
            break;
        }
        case 4: {
            BinaryFile<Time> timeFile("time.dat");
            timeFile.create();
            timeFile.openForWork();
            menu(timeFile, "Время с напоминанием");
            break;
        }
        default:
            cout << "Неверный тип данных!\n";
            return 1;
        }
    }
    catch (const std::bad_alloc& e) {
        cout << "Ошибка: недостаточно памяти: " << e.what() << "\n";
    }
    catch (const std::out_of_range& e) {
        cout << "Ошибка диапазона: " << e.what() << "\n";
    }
    catch (const std::invalid_argument& e) {
        cout << "Ошибка ввода: " << e.what() << "\n";
    }
    catch (const std::runtime_error& e) {
        cout << "Ошибка выполнения: " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        cout << "Неожиданная ошибка: " << e.what() << "\n";
    }
    catch (...) {
        cout << "Произошло неизвестное исключение.\n";
    }

    return 0;
}
