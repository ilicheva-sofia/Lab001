#pragma once
#include <cstdint>
#include <format>

#include "DomainError.hpp"

namespace Values {
    class Money {
       private:
        std::int64_t kopecks;
        // Приватный конструктор без проверок для безопасной сборки объектов
        // внутри исключений
        constexpr explicit Money(std::int64_t kopecks, bool /*unused*/)
            : kopecks(kopecks) {}

       public:
        constexpr Money() : kopecks(0) {}
        constexpr explicit Money(std::int64_t kopecks) : kopecks(kopecks) {
            if (kopecks < 0) {
                throw Exceptions::InvalidMoney(Money(kopecks, true));
            }
        }

        static constexpr Money zero() { return Money(); }

        constexpr std::int64_t get_kopecks() const { return kopecks; }

        bool operator==(const Money& other) const;
        bool operator>(const Money& other) const;
        bool operator<(const Money& other) const;
        bool operator>=(const Money& other) const;
        bool operator<=(const Money& other) const;

        Money operator+(const Money& other) const;
        Money operator-(const Money& other) const;
        Money& operator+=(const Money& other);
        Money& operator-=(const Money& other);
        Money operator*(std::int64_t factor) const;
    };
}  // namespace Values

namespace std {
    template <>
    struct formatter<Values::Money> : formatter<string_view> {
        auto format(const Values::Money& money, format_context& ctx) const {
            const auto kopecks = money.get_kopecks();
            return format_to(ctx.out(), "{}.{:02} RUB", kopecks / 100,
                             kopecks % 100);
        }
    };
}  // namespace std
