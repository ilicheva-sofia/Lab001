#include "Table.hpp"

#include <iostream>

#include "DomainError.hpp"

namespace Aggregates {
    Table::Table(Values::TableId id, Values::Money initial_balance,
                 Values::Tablelimits table_limits)
        : id(id), balance(initial_balance), limits(table_limits) {}

    void Table::accept_bet(Values::PlayerId id,
                           std::unique_ptr<Bets::Bet> bet) {
        limits.validate(bet->get_amount());

        balance += bet->get_amount();
        bets.push_back(PlacedBet{id, std::move(bet)});
    }

    Values::SpinResult Table::spin(Ports::RandomGenerator& rg) {
        Values::RouletteNumber winning_number = rg.next();

        std::vector<Values::PayoutEntry> payouts;
        Values::Money total_payout_needed = Values::Money::zero();

        for (const auto& placed_bet : bets) {
            Values::Money payout = placed_bet.bet->payout(winning_number);
            // add all besides of win to have logging outside
            payouts.push_back(Values::PayoutEntry(placed_bet.player, payout));

            total_payout_needed += payout;
        }

        if (total_payout_needed > balance) {
            roll_back();
            bets.clear();
            return Values::SpinResult(winning_number,
                                      Values::SpinStatus::Cancelled, {});
        }

        balance -= total_payout_needed;
        bets.clear();

        return Values::SpinResult(winning_number, Values::SpinStatus::Completed,
                                  payouts);
    }

    void Table::roll_back() {
        std::cout << "[ОТМЕНА ХОДА] У казино не хватило денег на выплаты! "
                     "Возврат ставок.\n";
        for (const auto& placed_bet : bets) {
            balance -= placed_bet.bet->get_amount();
        }
    }
}  // namespace Aggregates
