#pragma once

#include "Money.hpp"
#include "PlayerId.hpp"

namespace Values {

    class PayoutEntry {
       private:
        PlayerId player;
        Money amount;

       public:
        PayoutEntry(PlayerId player, Money amount)
            : player(std::move(player)), amount(std::move(amount)) {}
        bool operator==(const PayoutEntry& other) const {
            return player == other.player && amount == other.amount;
        }
        PlayerId get_player() { return player; }
        Money get_amount() { return amount; }
    };

}  // namespace Values
