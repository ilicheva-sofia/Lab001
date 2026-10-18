#include "Money.hpp"

#include "DomainError.hpp"

namespace Values {

    constexpr Money::Money(std::int64_t kopecks) : kopecks(kopecks) {
        if (kopecks < 0) {
            throw Exceptions::InvalidMoney(Money(kopecks, true));
        }
    }

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

    Money Money::operator+(const Money& other) const {
        return Money(kopecks + other.kopecks);
    }

    Money Money::operator-(const Money& other) const {
        if (kopecks < other.kopecks) {
            throw Exceptions::InvalidMoneyOperation(
                "Недостаточно средств на балансе");
        }
        return Money(kopecks - other.kopecks);
    }

    Money Money::operator*(std::int64_t factor) const {
        return Money(kopecks * factor);
    }
}  // namespace Values
