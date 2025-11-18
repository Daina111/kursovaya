#ifndef BINARY_FILE_H 
#define BINARY_FILE_H

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

using namespace std;

template<typename T>
class BinaryFile : public fstream {
private:
    struct FileHeader {
        int  capacity;        // Размерность массива указателей
        int  size;            // Текущее количество объектов
        long pointersOffset;  // Смещение массива указателей
    };

    FileHeader header{};
    string filename;

    void readHeader() {
        fstream::seekg(0, ios::beg);
        fstream::read(reinterpret_cast<char*>(&header), sizeof(FileHeader));
    }

    void writeHeader() {
        fstream::seekp(0, ios::beg);
        fstream::write(reinterpret_cast<const char*>(&header), sizeof(FileHeader));
        fstream::flush();
    }

    long* readPointersArray() {
        long* pointers = new long[header.capacity];
        fstream::seekg(header.pointersOffset, ios::beg);
        fstream::read(reinterpret_cast<char*>(pointers),
            header.capacity * sizeof(long));
        return pointers;
    }

    void writePointersArray(const long* pointers) {
        fstream::seekp(header.pointersOffset, ios::beg);
        fstream::write(reinterpret_cast<const char*>(pointers),
            header.capacity * sizeof(long));
        fstream::flush();
    }

    // Увеличение capacity в 2 раза и перенос массива указателей в конец файла
    void expandCapacity() {
        int newCapacity = header.capacity * 2;

        long* oldPointers = readPointersArray();
        long* newPointers = new long[newCapacity];

        for (int i = 0; i < header.size; ++i) {
            newPointers[i] = oldPointers[i];
        }
        for (int i = header.size; i < newCapacity; ++i) {
            newPointers[i] = -1;
        }

        // переносим массив указателей в КОНЕЦ файла
        fstream::seekp(0, ios::end);
        header.pointersOffset = fstream::tellp();
        header.capacity = newCapacity;

        writePointersArray(newPointers);
        writeHeader();

        delete[] oldPointers;
        delete[] newPointers;
    }

    void writeObject(const T& obj) {
        if constexpr (std::is_trivial_v<T>) {
            fstream::write(reinterpret_cast<const char*>(&obj), sizeof(T));
        }
        else {
            obj.serialize(*this);
        }
    }

    void readObject(T& obj) {
        if constexpr (std::is_trivial_v<T>) {
            fstream::read(reinterpret_cast<char*>(&obj), sizeof(T));
        }
        else {
            obj.deserialize(*this);
        }
    }

public:
    BinaryFile(const string& fname) : filename(fname) {}

    // Создание нового файла (обнуляем содержимое)
    bool create() {
        fstream::open(filename, ios::out | ios::binary | ios::trunc);
        if (!is_open()) return false;

        header.capacity = 10;
        header.size = 0;
        header.pointersOffset = sizeof(FileHeader);

        writeHeader();

        long* pointers = new long[header.capacity];
        for (int i = 0; i < header.capacity; ++i) {
            pointers[i] = -1;
        }
        writePointersArray(pointers);
        delete[] pointers;

        close();
        return true;
    }

    bool openForWork() {
        fstream::open(filename, ios::in | ios::out | ios::binary);
        if (!is_open()) return false;
        readHeader();
        return true;
    }

    void closeFile() {
        if (is_open()) close();
    }

    bool add(const T& obj) {
        if (header.size >= header.capacity) {
            expandCapacity();
        }

        // Пишем объект в конец файла
        fstream::seekp(0, ios::end);
        long objectOffset = fstream::tellp();
        writeObject(obj);

        // Обновляем массив указателей
        long* pointers = readPointersArray();
        pointers[header.size] = objectOffset;
        header.size++;

        writePointersArray(pointers);
        writeHeader();

        delete[] pointers;
        return true;
    }

    bool get(int index, T& obj) {
        if (index < 0 || index >= header.size) {
            throw std::out_of_range("Индекс вне диапазона в BinaryFile::get");
        }

        long* pointers = readPointersArray();
        long objectOffset = pointers[index];

        if (objectOffset == -1) {
            delete[] pointers;
            throw std::runtime_error("Объект помечен как удалён в BinaryFile::get");
        }

        fstream::seekg(objectOffset, ios::beg);
        readObject(obj);

        delete[] pointers;
        return true;
    }


    bool update(int index, const T& obj) {
        if (index < 0 || index >= header.size) {
            throw std::out_of_range("Индекс вне диапазона в BinaryFile::update");
        }

        long* pointers = readPointersArray();
        long objectOffset = pointers[index];

        if (objectOffset == -1) {
            delete[] pointers;
            throw std::runtime_error("Объект помечен как удалён в BinaryFile::update");
        }

        fstream::seekp(objectOffset, ios::beg);
        writeObject(obj);

        delete[] pointers;
        return true;
    }


    bool remove(int index) {
        if (index < 0 || index >= header.size) {
            throw std::out_of_range("Индекс вне диапазона в BinaryFile::remove");
        }

        long* pointers = readPointersArray();

        for (int i = index; i < header.size - 1; ++i) {
            pointers[i] = pointers[i + 1];
        }
        pointers[header.size - 1] = -1;
        header.size--;

        writePointersArray(pointers);
        writeHeader();

        delete[] pointers;
        return true;
    }


    // Безопасная сортировка: читаем все objs, сортируем в памяти,
    // пишем НОВЫЕ копии в конец файла и обновляем массив указателей
    void sort() {
        if (header.size <= 1) return;

        // 1) читаем объекты
        T* objects = new T[header.size];
        for (int i = 0; i < header.size; ++i) {
            get(i, objects[i]);
        }

        // 2) сортируем
        std::sort(objects, objects + header.size);

        // 3) записываем в конец файла отсортированные копии
        fstream::seekp(0, ios::end);
        long* pointers = new long[header.capacity];

        for (int i = 0; i < header.size; ++i) {
            long pos = fstream::tellp();
            writeObject(objects[i]);
            pointers[i] = pos;
        }

        // остаток заполняем -1
        for (int i = header.size; i < header.capacity; ++i) {
            pointers[i] = -1;
        }

        // 4) записываем обновлённый массив указателей
        writePointersArray(pointers);
        writeHeader();

        delete[] objects;
        delete[] pointers;
    }

    void displayAll() {
        cout << "Всего объектов: " << header.size << endl;
        for (int i = 0; i < header.size; ++i) {
            T obj;
            if (get(i, obj)) {
                cout << "[" << i << "] " << obj << endl;
            }
        }
    }

    int getSize() const { return header.size; }
};

#endif
