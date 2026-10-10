#include <cstdint>
#include <stdexcept>
#include "DomainError.hpp"

namespace Values{
    class RuletNumber {// класс выпавшего номера//
        private:
        std::int8_t number;
        public:
        constexpr RuletNumber(std::int8_t number) : number(number) {
        if ((number < 0) || (number > 36))
          throw  Exceptions::InvalidNumber(number);
      }

    };
}