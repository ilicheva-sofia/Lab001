#include "Player.hpp"

void Domain::Player::receive_payout(Money amount) {
    if (amount <= Money::zero()) {
        throw InvalidAmountException("Выигрыш должен быть положительным");
    }
    balance += amount;
}

// Размещение ставки
void Domain::Player::place_bet(Table& table, Bet& bet) {
    if (bet->amount() > balance) {
        throw InsufficientFundsException(amount, balance);
    }
    balance -= amount;
    table.accept_bet(*this, bet);
}