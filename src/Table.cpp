#include "Table.hpp"

#include "Bet.hpp"
#include "DomainError.hpp"
#include "RandomGenerator.hpp"

void Aggregates::Table::accept_bet(Values::PlayerId id,
                                   std::unique_ptr<Bets::Bet> bet) {
    auto m = bet->amount();
    if (!limits.accepts(m)) throw Exceptions::BetOutsideLimits(m);

    bets.push_back(PlacedBet{id, std::move(bet)});
}

SpinResult Aggregates::Table::spin(Ports::RandomGenerator& rg) {
    int raw_number = rg.next();
    Values::RouletteNumber winning_number(raw_number);
    std::vector<PayoutEntry> payouts;

    for (const auto& placed_bet : bets) {
        Money payout = placed_bet.bet->payout(winning_number);
        if (payout > Values::Money::zero()) {
            payouts.push_back({placed_bet.player, payout});
        }
    }

    bets.clear();
    return SpinResult{winning_number, payouts};
}
