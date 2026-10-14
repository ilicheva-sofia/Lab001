#include "Table.hpp"

#include "Bet.hpp"
#include "DomainError.hpp"
#include "RandomGenerator.hpp"

namespace Aggregates {
    void Table::accept_bet(Values::PlayerId id,
                           std::unique_ptr<Bets::Bet> bet) {
        auto m = bet->amount();
        if (!limits.accepts(m)) throw Exceptions::BetOutsideLimits(m);

        bets.push_back(PlacedBet{id, std::move(bet)});
    };
    SpinResult Table::spin(RandomGenerator& rg) {}
}  // namespace Aggregates
