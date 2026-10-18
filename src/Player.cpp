#include "Player.hpp"

#include "DomainError.hpp"

namespace Domain {

    Player::Player(Values::PlayerId id, Values::Money init_balance)
        : id(id), balance(init_balance) {}

    void Player::receive_payout(Values::Money amount) { balance += amount; }

    void Player::spend_money(Values::Money amount) {
        // operation-= throws InvalidMoneyOperation
        balance -= amount;
    }

}  // namespace Domain
