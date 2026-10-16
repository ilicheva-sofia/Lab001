#pragma once

#include "Money.hpp"
#include "PlayerId.hpp"

namespace Values {

    struct PayoutEntry {
        PlayerId player;
        Money amount;
        PayoutEntry(PlayerId player, Money amount)
            : player(std::move(player)), amount(std::move(amount)) {}
        bool operator==(const PayoutEntry& other) const {
            return player == other.player && amount == other.amount;
        }
    };

}  // namespace Values
