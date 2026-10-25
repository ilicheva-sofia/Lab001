#include "BotStrategy.hpp"

namespace Domain {

    BotStrategy::BotStrategy(const std::string& name,
                             Values::Money target_profit,
                             Values::Money stop_loss,
                             Values::Money initial_balance)
        : name(name),
          target_profit(target_profit),
          stop_loss(stop_loss),
          initial_balance(initial_balance) {}

    RedConstantBot::RedConstantBot(const std::string& name,
                                   Values::Money target_profit,
                                   Values::Money stop_loss,
                                   Values::Money initial_balance,
                                   Values::Money bet_amount)
        : BotStrategy(name, target_profit, stop_loss, initial_balance),
          m_bet_amount(bet_amount) {}

    BotDecision RedConstantBot::make_decision(
        const Domain::Player& player, const Values::Tablelimits& limits) {
        Values::Money current_balance = player.get_balance();

        if (current_balance > initial_balance + target_profit ||
            current_balance == initial_balance + target_profit) {
            return BotDecision{false, nullptr};  // Уходим в плюсе!
        }

        if (initial_balance > current_balance &&
            (initial_balance - current_balance > stop_loss ||
             initial_balance - current_balance == stop_loss)) {
            return BotDecision{false,
                               nullptr};  // Уходим, чтобы не проиграть всё
        }

        if (current_balance < m_bet_amount) {
            return BotDecision{false, nullptr};  // Физически нет денег
        }

        if (!limits.accepts(m_bet_amount)) {
            return BotDecision{false, nullptr};
        }

        return BotDecision{true, std::make_unique<Bets::ColorBet>(
                                     m_bet_amount, Values::Color::Red)};
    }

    MartingaleBot::MartingaleBot(const std::string& name,
                                 Values::Money target_profit,
                                 Values::Money stop_loss,
                                 Values::Money initial_balance,
                                 Values::Money base_bet, Values::Color color)
        : BotStrategy(name, target_profit, stop_loss, initial_balance),
          m_base_bet(base_bet),
          m_current_bet(base_bet),
          m_color(color) {}

    BotDecision MartingaleBot::make_decision(
        const Domain::Player& player, const Values::Tablelimits& limits) {
        Values::Money current_balance = player.get_balance();

        if (current_balance > initial_balance + target_profit ||
            current_balance == initial_balance + target_profit) {
            return BotDecision{false, nullptr};
        }

        if (initial_balance > current_balance &&
            (initial_balance - current_balance > stop_loss ||
             initial_balance - current_balance == stop_loss)) {
            return BotDecision{false, nullptr};
        }

        if (current_balance < m_current_bet) {
            // Если на удвоенную ставку денег не хватает, стратегия Мартингейла
            // "ломается". Бот вынужден признать поражение и покинуть стол.
            return BotDecision{false, nullptr};
        }

        if (!limits.accepts(m_current_bet)) {
            return BotDecision{false, nullptr};
        }

        return BotDecision{
            true, std::make_unique<Bets::ColorBet>(m_current_bet, m_color)};
    }

    void MartingaleBot::update_result(const Values::SpinResult& result,
                                      Values::PlayerId pid) {
        if (result.get_status() == Values::SpinStatus::Cancelled) {
            return;
        }

        bool won = false;

        for (const auto& payout : result.get_payouts()) {
            if (payout.player.get_id() == pid.get_id() &&
                payout.amount > Values::Money::zero()) {
                won = true;
                break;
            }
        }

        if (won) {
            m_current_bet = m_base_bet;
        } else {
            m_current_bet = m_current_bet * 2;
        }
    }

}  // namespace Domain
