#pragma once
#include <memory>
#include <string>

#include "Bet.hpp"
#include "Money.hpp"
#include "Player.hpp"
#include "SpinResult.hpp"
#include "Tablelimits.hpp"

namespace Domain {

    // Структура, которую возвращает бот на каждом шаге симуляции
    struct BotDecision {
        bool wants_to_play;  // Продолжает ли игру или уходит (достиг лимитов)
        std::unique_ptr<Bets::Bet>
            bet;  // Какую ставку делает (nullptr, если уходит)
    };

    class BotStrategy {
       protected:
        Values::Money m_target_profit;    // Цель по выигрышу (Take-Profit)
        Values::Money m_stop_loss;        // Лимит потерь (Stop-Loss)
        Values::Money m_initial_balance;  // С чем бот зашел за стол
        std::string m_name;

       public:
        BotStrategy(const std::string& name, Values::Money target_profit,
                    Values::Money stop_loss, Values::Money initial_balance);
        virtual ~BotStrategy() = default;

        std::string get_name() const { return m_name; }
        // Главный метод: бот оценивает баланс игрока и лимиты стола, а затем
        // принимает решение
        virtual BotDecision make_decision(
            const Domain::Player& player,
            const Values::Tablelimits& limits) = 0;

        virtual void update_result(const Values::SpinResult& result,
                                   Values::PlayerId pid) {}
    };

    // Бот, который всегда ставит на Красное
    class RedConstantBot : public BotStrategy {
       private:
        Values::Money m_bet_amount;

       public:
        RedConstantBot(const std::string& name, Values::Money target_profit,
                       Values::Money stop_loss, Values::Money initial_balance,
                       Values::Money bet_amount);

        BotDecision make_decision(const Domain::Player& player,
                                  const Values::Tablelimits& limits) override;
    };

    // Бот, который всегда ставит на черное с удвоением ставок
    class MartingaleBot : public BotStrategy {
       private:
        Values::Money m_base_bet;  // Стартовая (базовая) ставка
        Values::Money
            m_current_bet;      // Текущая ставка (которая может удваиваться)
        Values::Color m_color;  // На какой цвет всегда ставим (обычно Black)

       public:
        MartingaleBot(const std::string& name, Values::Money target_profit,
                      Values::Money stop_loss, Values::Money initial_balance,
                      Values::Money base_bet,
                      Values::Color color = Values::Color::Black);

        BotDecision make_decision(const Domain::Player& player,
                                  const Values::Tablelimits& limits) override;

        // Переопределяем метод, чтобы анализировать выигрыш/проигрыш
        void update_result(const Values::SpinResult& result,
                           Values::PlayerId pid) override;
    };
}  // namespace Domain
