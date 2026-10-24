#include "BotStrategy.hpp"

namespace Domain {

    BotStrategy::BotStrategy(const std::string& name,
                             Values::Money target_profit,
                             Values::Money stop_loss,
                             Values::Money initial_balance)
        : m_name(name),
          m_target_profit(target_profit),
          m_stop_loss(stop_loss),
          m_initial_balance(initial_balance) {}

    // === Реализация RedConstantBot ===
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

        // 1. Проверяем цель по выигрышу (Take-Profit)
        // Если текущий баланс >= начальный + цель
        if (current_balance > m_initial_balance + m_target_profit ||
            current_balance == m_initial_balance + m_target_profit) {
            return BotDecision{false, nullptr};  // Уходим в плюсе!
        }

        // 2. Проверяем лимит потерь (Stop-Loss)
        // Если проиграли больше допустимого (текущий баланс <= начальный -
        // stop_loss)
        if (m_initial_balance > current_balance &&
            (m_initial_balance - current_balance > m_stop_loss ||
             m_initial_balance - current_balance == m_stop_loss)) {
            return BotDecision{false,
                               nullptr};  // Уходим, чтобы не проиграть всё
        }

        // 3. Проверяем, хватает ли вообще денег у игрока на следующую ставку
        if (current_balance < m_bet_amount) {
            return BotDecision{false, nullptr};  // Физически нет денег
        }

        // 4. Проверяем, подходит ли наша ставка под лимиты стола
        if (!limits.accepts(m_bet_amount)) {
            return BotDecision{false,
                               nullptr};  // Ставка нарушает правила стола
        }

        // 5. Если все проверки пройдены — делаем ставку!
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

        // 1. Проверяем Take-Profit
        if (current_balance > m_initial_balance + m_target_profit ||
            current_balance == m_initial_balance + m_target_profit) {
            return BotDecision{false, nullptr};
        }

        // 2. Проверяем Stop-Loss
        if (m_initial_balance > current_balance &&
            (m_initial_balance - current_balance > m_stop_loss ||
             m_initial_balance - current_balance == m_stop_loss)) {
            return BotDecision{false, nullptr};
        }

        // 3. Проверяем, хватает ли денег на ТЕКУЩУЮ (возможно, уже удвоенную)
        // ставку
        if (current_balance < m_current_bet) {
            // Если на удвоенную ставку денег не хватает, стратегия Мартингейла
            // "ломается". Бот вынужден признать поражение и покинуть стол.
            return BotDecision{false, nullptr};
        }

        // 4. Проверяем лимиты стола для текущей ставки
        if (!limits.accepts(m_current_bet)) {
            // Если стол не принимает такую большую ставку (уперлись в MaxBet
            // стола), бот тоже останавливает симуляцию.
            return BotDecision{false, nullptr};
        }

        // 5. Делаем ставку на выбранный цвет
        return BotDecision{
            true, std::make_unique<Bets::ColorBet>(m_current_bet, m_color)};
    }

    void MartingaleBot::update_result(const Values::SpinResult& result,
                                      Values::PlayerId pid) {
        // Если раунд был отменен (например, форс-мажор стола), ставку не меняем
        if (result.get_status() == Values::SpinStatus::Cancelled) {
            return;
        }

        bool won = false;

        // Ищем себя в списке выплат раунда
        for (const auto& payout : result.get_payouts()) {
            if (payout.player.get_id() == pid.get_id() &&
                payout.amount > Values::Money::zero()) {
                won = true;
                break;
            }
        }

        if (won) {
            // Если ВЫИГРАЛИ — откатываемся к базовой ставке
            m_current_bet = m_base_bet;
        } else {
            // Если ПРОИГРАЛИ — удваиваем ставку для следующего раунда!
            m_current_bet = m_current_bet * 2;
        }
    }

}  // namespace Domain
