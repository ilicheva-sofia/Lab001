#include "DomainError.hpp"

#include <format>

#include "Money.hpp"  // Теперь это безопасно!

namespace Exceptions {
    DomainError::DomainError(const std::string& message)
        : std::runtime_error(message) {}

    InvalidMoney::InvalidMoney(const Values::Money& money)
        : DomainError(std::format(
              "Количество денег не может быть отрицательным: {}", money)) {}

    InvalidMoneyOperation::InvalidMoneyOperation(const std::string& msg)
        : DomainError(msg) {}

    InvalidNumber::InvalidNumber(std::int8_t number)
        : DomainError(std::format("Номер должен быть от 0 до 36: {}", number)) {
    }

    // Реализация ошибки лимитов с полноценными объектами Money
    InvalidTablelimits::InvalidTablelimits(const Values::Money& min_money,
                                           const Values::Money& max_money)
        : DomainError(
              std::format("Минимум не может быть больше максимума: {} и {}",
                          min_money, max_money)) {}

    InvalidPlayerId::InvalidPlayerId(std::int64_t id)
        : DomainError(std::format("Неправильное значение для игрока: {}", id)) {
    }

    BetOutsideLimits::BetOutsideLimits(const Values::Money& money)
        : DomainError(std::format("Ставка вне лимитов стола: {}", money)) {}
}  // namespace Exceptions
