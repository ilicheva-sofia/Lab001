#pragma once
#include <cstdint>
#include <stdexcept>

#include "DomainError.hpp"

namespace Values {
    enum class Color { Red, Black, Green };
    class RouletteNumber {  // класс выпавшего номера
       private:
        std::int8_t number;

       public:
        constexpr RouletteNumber(std::int8_t number) : number(number) {
            if ((number < 0) || (number > 36))
                throw Exceptions::InvalidNumber(number);
        }
        int value() const { return number; }
        Color color() const {
            if (number == 0) return Color::Green;

            // Определяем цвет по диапазону
            if ((number >= 1 && number <= 10) ||
                (number >= 19 && number <= 28)) {
                // В этих диапазонах: нечётные - красные, чётные - чёрные
                return (number % 2 == 1) ? Color::Red : Color::Black;
            } else {
                // В остальных диапазонах: нечётные - чёрные, чётные - красные
                return (number % 2 == 1) ? Color::Black : Color::Red;
            }
        }

        bool operator==(const RouletteNumber& other) const {
            return number == other.number;
        }
    };
}  // namespace Values
