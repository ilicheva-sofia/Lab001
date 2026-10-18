#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>

// Опережающее объявление (Forward declaration)
namespace Values {
    class Money;
}

namespace Exceptions {
    class DomainError : public std::runtime_error {
       public:
        explicit DomainError(const std::string& message);
    };

    class InvalidMoney : public DomainError {
       public:
        explicit InvalidMoney(const Values::Money& money);
    };

    class InvalidMoneyOperation : public DomainError {
       public:
        explicit InvalidMoneyOperation(const std::string& msg);
    };

    class InvalidNumber : public DomainError {
       public:
        explicit InvalidNumber(std::int8_t number);
    };

    // Принимаем ссылки на Money! Чистый DDD подход
    class InvalidTablelimits : public DomainError {
       public:
        InvalidTablelimits(const Values::Money& min_money,
                           const Values::Money& max_money);
    };

    class InvalidPlayerId : public DomainError {
       public:
        explicit InvalidPlayerId(std::int64_t id);
    };

    class BetOutsideLimits : public DomainError {
       public:
        BetOutsideLimits(const Values::Money& money);
    };
}  // namespace Exceptions
