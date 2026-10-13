#pragma once
#include <cstdint>
#include <format>

#include "DomainError.hpp"

namespace Values {

    class Money {
       private:
        std::int64_t kopecks;

       public:
        constexpr Money(std::int64_t kopecks) : kopecks(kopecks) {
            if (kopecks < 0) throw Exceptions::InvalidMoney(kopecks);
        }

        std::int64_t get_kopecks() const { return kopecks; }

        bool operator==(const Money& other) const {
            return kopecks == other.kopecks;
        }

        bool operator>(const Money& other) const {
            return kopecks > other.kopecks;
        }

        bool operator<(const Money& other) const {
            return kopecks < other.kopecks;
        }

        Money operator+(const Money& other) const {
            return {kopecks + other.kopecks};
        }

        Money operator-(const Money& other) const {
            return {kopecks - other.kopecks};
        }
    };
}  // namespace Values

namespace std {
    template <>
    struct formatter<Values::Money> : formatter<string_view> {
        constexpr auto format(const Values::Money& money,
                              format_context& ctx) const {
            const auto kopecks = money.get_kopecks();
            return format_to(ctx.out(), "{}.{:02} RUB", kopecks / 100,
                             kopecks % 100);
        }
    };
}  // namespace std
