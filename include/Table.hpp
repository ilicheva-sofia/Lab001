#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Bet.hpp"
#include "Money.hpp"
#include "PayoutEntry.hpp"
#include "PlayerId.hpp"
#include "RandomGenerator.hpp"
#include "SpinResult.hpp"
#include "TableId.hpp"
#include "Tablelimits.hpp"

namespace Aggregates {

    struct PlacedBet {
        Values::PlayerId player;
        std::unique_ptr<Bets::Bet> bet;
    };

    class Table {
       private:
        Values::TableId id;
        std::vector<PlacedBet> bets;
        Values::Money balance;
        Values::Tablelimits limits;

        void roll_back();

       public:
        Table(Values::TableId id, Values::Money initial_balance,
              Values::Tablelimits table_limits);

        Values::TableId get_id() const { return id; }

        void accept_bet(Values::PlayerId id, std::unique_ptr<Bets::Bet> bet);
        Values::SpinResult spin(Ports::RandomGenerator& rg);

        Values::Money get_balance() const { return balance; }
        const Values::Tablelimits& get_limits() const { return limits; }
    };
}  // namespace Aggregates
