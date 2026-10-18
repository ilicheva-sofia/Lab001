#include "Money.hpp"

#include "DomainError.hpp"  // Теперь подключаем здесь — безопасно!

namespace Values {

    constexpr Money::Money() : kopecks(0) {}

    constexpr Money::Money(std::int64_t kopecks) : kopecks(kopecks) {
        if (kopecks < 0) {
            // Передаем объект Money, созданный через приватный "безопасный"
            // конструктор
            throw Exceptions::InvalidMoney(Money(kopecks, true));
        }
    }

    Money Money::zero() { return Money(0); }

    std::int64_t Money::get_kopecks() const { return kopecks; }

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
                "Недостаточно средств на балансе");  // [2]
        }
        return Money(kopecks - other.kopecks);
    }

    Money Money::operator*(std::int64_t factor) const {
        return Money(kopecks * factor);
    }
}  // namespace Values
