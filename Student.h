#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <string>

class Student {
private:
    std::string name;
    int age;
    double averageGrade;

    // Проверка корректности возраста и среднего балла
    void ValidateData(int a, double g);

public:
    Student();
    Student(const std::string& n, int a, double g);

    bool operator<(const Student& other) const;
    bool operator>(const Student& other) const;
    bool operator==(const Student& other) const;

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    friend std::ostream& operator<<(std::ostream& os, const Student& student);
    friend std::istream& operator>>(std::istream& is, Student& student);

    std::string getName() const;
    int getAge() const;
    double getAverageGrade() const;
};

#endif
