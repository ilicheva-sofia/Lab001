#pragma once
#include <cstdint>
#include <format>
#include <stdexcept>

#include "Money.hpp"

namespace Exceptions {
    using Values::Money;
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

    class InvalidMoneyOperation : public DomainError {
        std::int64_t kopecks;

       public:
        InvalidMoneyOperation(std::int64_t kopecks)
            : DomainError(std::format(" Недостаточно средств : {}", kopecks)),
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
            : DomainError(
                  std::format("минимум не может быть больше максимума : {}, {}",
                              min_money, max_money)),
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

    class InvalidTableId : public DomainError {
        std::int64_t number;

       public:
        InvalidTableId(std::int64_t number)
            : DomainError(
                  std::format("неправильное значение для стола : {}", number)),
              number(number) {}
    };

    class BetOutsideLimits : public DomainError {
        Money money;

       public:
        BetOutsideLimits(Money money)
            : DomainError(std::format("ставка вне лимитов стола : {}", money)),
              money(money) {}
    };

    class FileLoggerError : public DomainError {
        std::string filePath;

       public:
        FileLoggerError(const std::string& path)
            : DomainError(
                  std::format("не удалось открыть файл лога: {}", path)),
              filePath(path) {}
    };
}  // namespace Exceptions
