#include <cstdint>
#include <stdexcept>
#include "DomainError.hpp"


namespace Values{

    class Money {
        private:
        std::int64_t kopecks;
        public:
        constexpr Money(std::int64_t kopecks) : kopecks(kopecks) {
        if (kopecks < 0)
          throw  Exceptions::InvalidMoney(kopecks);
      }

      std::int64_t get_kopecks() const { return kopecks; }

      bool operator==(const Money &other) const {
        return kopecks == other.kopecks;
      }

      bool operator>(const Money &other) const {
        return kopecks > other.kopecks;
      }

      Money operator+(const Money &other) const {
        return {kopecks + other.kopecks};
      }

      Money operator-(const Money &other) const {
        return {kopecks - other.kopecks};
      }
    };
}

