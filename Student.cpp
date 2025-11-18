#include "Student.h"
#include <limits>
#include <stdexcept>
#include <string>
#include <sstream>
using namespace std;

void Student::ValidateData(int a, double g) {
    if (a < 0 || a > 120) {
        throw invalid_argument("Возраст должен быть в диапазоне 0–120.");
    }
    if (g < 0.0 || g > 5.0) {
        throw invalid_argument("Средний балл должен быть в диапазоне 0–5.");
    }
}

Student::Student() : name(""), age(0), averageGrade(0.0) {}

Student::Student(const string& n, int a, double g)
    : name(n), age(0), averageGrade(0.0)
{
    ValidateData(a, g);
    age = a;
    averageGrade = g;
}

bool Student::operator<(const Student& other) const {
    return averageGrade < other.averageGrade;
}

bool Student::operator>(const Student& other) const {
    return averageGrade > other.averageGrade;
}

bool Student::operator==(const Student& other) const {
    return name == other.name &&
        age == other.age &&
        averageGrade == other.averageGrade;
}

ostream& operator<<(ostream& os, const Student& student) {
    os << "Студент: " << student.name
        << ", Возраст: " << student.age
        << ", Средний балл: " << student.averageGrade;
    return os;
}

istream& operator>>(istream& is, Student& student) {
    using std::numeric_limits;
    using std::streamsize;

    cout << "Введите имя: ";
    getline(is >> ws, student.name);

    string ageStr, gradeStr;

    cout << "Введите возраст: ";
    is >> ageStr;

    cout << "Введите средний балл: ";
    is >> gradeStr;

    int age;
    double grade;

    // --- разбираем возраст ---
    {
        stringstream ss(ageStr);
        if (!(ss >> age)) {
            throw invalid_argument(
                "Некорректный ввод данных студента: возраст должен быть целым числом."
            );
        }

        char extra;
        if (ss >> extra) {  // смогли прочитать что-то ещё после числа
            throw invalid_argument(
                "Некорректный ввод данных студента: возраст содержит лишние символы."
            );
        }
    }

    // --- разбираем средний балл ---
    {
        stringstream ss(gradeStr);
        if (!(ss >> grade)) {
            throw invalid_argument(
                "Некорректный ввод данных студента: средний балл должен быть числом."
            );
        }

        char extra;
        if (ss >> extra) {
            throw invalid_argument(
                "Некорректный ввод данных студента: средний балл содержит лишние символы."
            );
        }
    }

    // проверка диапазона (0–120, 0–5)
    student.ValidateData(age, grade);

    // если всё ок — сохраняем
    student.age = age;
    student.averageGrade = grade;

    // подчистим хвост строки из основного cin
    is.ignore(numeric_limits<streamsize>::max(), '\n');
    return is;
}

void Student::serialize(ostream& os) const {
    string utf8_name = name;
    size_t nameLength = utf8_name.length();
    os.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
    os.write(utf8_name.c_str(), nameLength);

    os.write(reinterpret_cast<const char*>(&age), sizeof(age));
    os.write(reinterpret_cast<const char*>(&averageGrade), sizeof(averageGrade));
}

void Student::deserialize(istream& is) {
    size_t nameLength;
    is.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

    char* buffer = new char[nameLength + 1];
    is.read(buffer, nameLength);
    buffer[nameLength] = '\0';
    name = string(buffer);
    delete[] buffer;

    is.read(reinterpret_cast<char*>(&age), sizeof(age));
    is.read(reinterpret_cast<char*>(&averageGrade), sizeof(averageGrade));

    // Проверим, что из файла не прочитали бред
    ValidateData(age, averageGrade);
}

string Student::getName() const { return name; }
int Student::getAge() const { return age; }
double Student::getAverageGrade() const { return averageGrade; }
