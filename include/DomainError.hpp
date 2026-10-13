#pragma once
#include <cstdint>
#include <format>
#include <stdexcept>

#include "Money.hpp"

using namespace Values;

namespace Exceptions {
    class DomainError : public std::runtime_error {
       public:
        explicit DomainError(const std::string& message)
            : std::runtime_error(message) {}
    };

    class InvalidMoney : public DomainError {
        std::int64_t kopecks;

       public:
        InvalidMoney(std::int64_t kopecks)
            : DomainError(std::format(
                  " количество денег не может быть отрицательным : {}",
                  kopecks)),
              kopecks(kopecks) {}
    };

    class InvalidNumber : public DomainError {
        std::int8_t number;

       public:
        InvalidNumber(std::int8_t number)
            : DomainError(std::format(
                  " номер не может быть меньше 0 и больше 36 : {}", number)),
              number(number) {}
    };

    class InvalidTablelimits : public DomainError {
        Money min_money;
        Money max_money;

       public:
        InvalidTablelimits(Money min_money, Money max_money)
            : DomainError(std::format(
                  " минимум не может быть больше максимума : {}, {}", min_money,
                  max_money)),
              min_money(min_money),
              max_money(max_money) {}
    };

    class InvalidPlayerId : public DomainError {
        std::int64_t number;

       public:
        InvalidPlayerId(std::int64_t number)
            : DomainError(
                  std::format("неправильное значение для игрока : {}", number)),
              number(number) {}
    };
    class InvalidBet : public DomainError {};
}  // namespace Exceptions
