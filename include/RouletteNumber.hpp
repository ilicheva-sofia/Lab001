#pragma once
#include <cstdint>

namespace Values {
    enum class Color { Red, Black, Green };

    class RouletteNumber {
       private:
        std::int8_t numer;

       public:
        // Конструктор оставляем в .hpp без constexpr, так как внутри есть throw
        // рантайм-ошибки
        RouletteNumber(std::int8_t number);

        std::int8_t value() const { return numer; }

        // Логику цвета можно сделать constexpr/inline методом
        Color color() const {
            if (numer == 0) return Color::Green;

            if ((numer >= 1 && numer <= 10) || (numer >= 19 && numer <= 28)) {
                return (numer % 2 == 1) ? Color::Red : Color::Black;
            } else {
                return (numer % 2 == 1) ? Color::Black : Color::Red;
            }
        }

        bool operator==(const RouletteNumber& other) const {
            return numer == other.numer;
        }
    };
}  // namespace Values
