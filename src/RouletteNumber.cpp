#include "RouletteNumber.hpp"

#include "DomainError.hpp"

namespace Values {
    RouletteNumber::RouletteNumber(std::int8_t number) : numer(number) {
        if (number < 0 || number > 36) {
            throw Exceptions::InvalidNumber(number);
        }
    }
}  // namespace Values
