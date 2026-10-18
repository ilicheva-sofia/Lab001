#pragma once
#include <cstdint>
#include <format>

namespace Exceptions {
    class InvalidMoney;
}

namespace Values {
    class Money {
       private:
        std::int64_t kopecks;

        // Приватный конструктор без проверок для безопасной сборки объектов
        // внутри исключений
        constexpr explicit Money(std::int64_t kopecks, bool /*unused*/)
            : kopecks(kopecks) {}

       public:
        constexpr Money();
        constexpr explicit Money(std::int64_t kopecks);

        static Money zero();
        std::int64_t get_kopecks() const;

        bool operator==(const Money& other) const;
        bool operator>(const Money& other) const;
        bool operator<(const Money& other) const;
        bool operator>=(const Money& other) const;
        bool operator<=(const Money& other) const;

        Money operator+(const Money& other) const;
        Money operator-(const Money& other) const;
        Money& operator+=(const Money& other) { return *this = *this + other; }
        Money& operator-=(const Money& other) { return *this = *this - other; }
        Money operator*(std::int64_t factor) const;
    };
}  // namespace Values

namespace std {
    template <>
    struct formatter<Values::Money> : formatter<string_view> {
        constexpr auto format(const Values::Money& money,
                              format_context& ctx) const {
            const auto kopecks = money.get_kopecks();
            return format_to(ctx.out(), "{}.{:02} RUB", kopecks / 100,
                             kopecks % 100);  // [3]
        }
    };
}  // namespace std
