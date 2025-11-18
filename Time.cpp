#include "Time.h"
#include <cstring>      // strlen, memcpy, strcmp
#include <limits>       // numeric_limits
#include <string>       // std::string
#include <stdexcept>    // std::invalid_argument, std::runtime_error
#include <iostream>

using namespace std;

// --- вспомогательные методы ---

void Time::copyReminder(const char* r) {
    delete[] reminder;

    if (r) {
        size_t len = std::strlen(r);
        reminder = new char[len + 1];
        std::memcpy(reminder, r, len + 1); // копируем строку вместе с '\0'
    }
    else {
        reminder = nullptr;
    }
}

// ТВОЯ функция проверки
void Time::ValidateTime(int h, int m, int s) {
    if (h < 0 || h > 23)
        throw std::invalid_argument("Часы должны быть в диапазоне от 0 до 23");
    if (m < 0 || m > 59)
        throw std::invalid_argument("Минуты должны быть в диапазоне от 0 до 59");
    if (s < 0 || s > 59)
        throw std::invalid_argument("Секунды должны быть в диапазоне от 0 до 59");
}

// --- конструкторы/деструктор/присваивание ---

Time::Time()
    : hours(0), minutes(0), seconds(0), reminder(nullptr) {
}

Time::Time(int h, int m, int s)
    : hours(0), minutes(0), seconds(0), reminder(nullptr)
{
    ValidateTime(h, m, s);
    hours = h;
    minutes = m;
    seconds = s;
}

Time::Time(int h, int m, int s, const char* r)
    : hours(0), minutes(0), seconds(0), reminder(nullptr)
{
    ValidateTime(h, m, s);
    hours = h;
    minutes = m;
    seconds = s;
    copyReminder(r);
}

Time::Time(const Time& other)
    : hours(other.hours),
    minutes(other.minutes),
    seconds(other.seconds),
    reminder(nullptr)
{
    copyReminder(other.reminder);
}

Time& Time::operator=(const Time& other) {
    if (this != &other) {
        hours = other.hours;
        minutes = other.minutes;
        seconds = other.seconds;
        copyReminder(other.reminder);
    }
    return *this;
}

Time::~Time() {
    delete[] reminder;
}

// --- сеттеры / геттеры ---

void Time::SetHours(int h) {
    // проверяем все компоненты времени
    ValidateTime(h, minutes, seconds);
    hours = h;
}

void Time::SetMinutes(int m) {
    ValidateTime(hours, m, seconds);
    minutes = m;
}

void Time::SetSeconds(int s) {
    ValidateTime(hours, minutes, s);
    seconds = s;
}

void Time::SetReminder(const char* r) {
    copyReminder(r);
}

int Time::GetHours()   const { return hours; }
int Time::GetMinutes() const { return minutes; }
int Time::GetSeconds() const { return seconds; }
const char* Time::GetReminder() const { return reminder; }

// --- вывод ---

void Time::Print() const {
    cout << (hours < 10 ? "0" : "") << hours << ":"
        << (minutes < 10 ? "0" : "") << minutes << ":"
        << (seconds < 10 ? "0" : "") << seconds;
    if (reminder) {
        cout << " - " << reminder;
    }
    cout << endl;
}


// --- сравнение ---

bool Time::operator<(const Time& other) const {
    if (hours != other.hours)   return hours < other.hours;
    if (minutes != other.minutes) return minutes < other.minutes;
    return seconds < other.seconds;
}

bool Time::operator>(const Time& other) const {
    return other < *this;
}

bool Time::operator==(const Time& other) const {
    bool sameTime = (hours == other.hours) &&
        (minutes == other.minutes) &&
        (seconds == other.seconds);

    if (!sameTime) return false;

    if (reminder == nullptr && other.reminder == nullptr) return true;
    if (reminder == nullptr || other.reminder == nullptr) return false;

    return std::strcmp(reminder, other.reminder) == 0;
}

// --- сериализация / десериализация ---



void Time::serialize(ostream& os) const {
    size_t len = 0;
    if (reminder) {
        len = std::strlen(reminder);
    }

    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        os.write(reminder, len);
    }

    os.write(reinterpret_cast<const char*>(&hours), sizeof(hours));
    os.write(reinterpret_cast<const char*>(&minutes), sizeof(minutes));
    os.write(reinterpret_cast<const char*>(&seconds), sizeof(seconds));
}

void Time::deserialize(istream& is) {
    size_t len = 0;
    is.read(reinterpret_cast<char*>(&len), sizeof(len));

    char* buf = nullptr;
    if (len > 0) {
        buf = new char[len + 1];
        is.read(buf, len);
        buf[len] = '\0';
    }

    copyReminder(buf);
    delete[] buf;

    is.read(reinterpret_cast<char*>(&hours), sizeof(hours));
    is.read(reinterpret_cast<char*>(&minutes), sizeof(minutes));
    is.read(reinterpret_cast<char*>(&seconds), sizeof(seconds));

    // можно оставить проверку корректности времени:
    ValidateTime(hours, minutes, seconds);
}


// --- оператор вывода ---

ostream& operator<<(ostream& os, const Time& t) {
    os << (t.hours < 10 ? "0" : "") << t.hours << ":"
        << (t.minutes < 10 ? "0" : "") << t.minutes << ":"
        << (t.seconds < 10 ? "0" : "") << t.seconds;

    os << " - ";
    if (t.reminder) os << t.reminder;
    else            os << "(нет напоминания)";

    return os;
}

// --- оператор ввода ---

istream& operator>>(istream& is, Time& t) {
    using std::numeric_limits;
    using std::streamsize;

    int h, m, s;

    cout << "Введите часы (0-23): ";
    if (!(is >> h)) {
        is.clear();
        is.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Некорректный ввод времени: часы должны быть целым числом.");
    }

    cout << "Введите минуты (0-59): ";
    if (!(is >> m)) {
        is.clear();
        is.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Некорректный ввод времени: минуты должны быть целым числом.");
    }

    cout << "Введите секунды (0-59): ";
    if (!(is >> s)) {
        is.clear();
        is.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Некорректный ввод времени: секунды должны быть целым числом.");
    }

    // тут уже проверяем диапазон
    t.ValidateTime(h, m, s);

    t.hours = h;
    t.minutes = m;
    t.seconds = s;

    // читаем напоминание
    is.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Введите напоминание: ";
    std::string note;
    std::getline(is, note);
    t.SetReminder(note.c_str());

    return is;
}

