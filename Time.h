#ifndef TIME_H
#define TIME_H

#include <iostream>

class Time {
private:
    int hours;
    int minutes;
    int seconds;
    char* reminder;   // динамическая строка

    void copyReminder(const char* r);
    void ValidateTime(int h, int m, int s);   // <-- твоя функция проверки

public:
    Time();
    Time(int h, int m, int s);
    Time(int h, int m, int s, const char* r);
    Time(const Time& other);
    Time& operator=(const Time& other);
    ~Time();

    // сеттеры
    void SetHours(int h);
    void SetMinutes(int m);
    void SetSeconds(int s);
    void SetReminder(const char* r);

    // геттеры
    int GetHours()   const;
    int GetMinutes() const;
    int GetSeconds() const;
    const char* GetReminder() const;

    void Print() const;

    // сравнение для сортировки
    bool operator<(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator==(const Time& other) const;

    // для BinaryFile
    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    // ввод/вывод
    friend std::ostream& operator<<(std::ostream& os, const Time& t);
    friend std::istream& operator>>(std::istream& is, Time& t);
};

#endif
