#pragma once
#include <cstdint>
#include <stdexcept>

#include "DomainError.hpp"

namespace Values {
    enum class Color { Red, Black, Green };
    class RouletteNumber {  // класс выпавшего номера//
       private:
        std::int8_t number;

       public:
        constexpr RouletteNumber(std::int8_t number) : number(number) {
            if ((number < 0) || (number > 36))
                throw Exceptions::InvalidNumber(number);
        }
        int value() const { return number; }
        Color color() const;  // RED, BLACK, GREEN (для 0)
        bool operator==(const RouletteNumber& other) const {
            return number == other.number;
        }
    };
}  // namespace Values
