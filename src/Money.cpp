#include "Money.hpp"

namespace Values {

    bool Money::operator==(const Money& other) const {
        return kopecks == other.kopecks;
    }
    bool Money::operator>(const Money& other) const {
        return kopecks > other.kopecks;
    }
    bool Money::operator>=(const Money& other) const {
        return kopecks >= other.kopecks;
    }
    bool Money::operator<(const Money& other) const {
        return kopecks < other.kopecks;
    }
    bool Money::operator<=(const Money& other) const {
        return kopecks <= other.kopecks;
    }

    Money& Money::operator+=(const Money& other) {
        kopecks += other.kopecks;
        return *this;
    }

    Money& Money::operator-=(const Money& other) {
        if (kopecks < other.kopecks) {
            throw Exceptions::InvalidMoneyOperation(
                "Недостаточно средств на балансе");
        }
        kopecks -= other.kopecks;
        return *this;
    }

    Money Money::operator+(const Money& other) const {
        Money result = *this;
        result += other;
        return result;
    }

    Money Money::operator-(const Money& other) const {
        Money result = *this;
        result -= other;
        return result;
    }

    Money Money::operator*(std::int64_t factor) const {
        return Money(kopecks * factor);
    }
}  // namespace Values
