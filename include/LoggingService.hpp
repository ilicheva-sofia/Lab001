#pragma once
#include <format>
#include <string>

#include "Bet.hpp"
#include "Logger.hpp"
#include "Money.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
#include "RouletteNumber.hpp"
#include "Table.hpp"

namespace Infrastructure {
    class LoggingService {
       private:
        Ports::Logger& logger;

       public:
        explicit LoggingService(Ports::Logger& logger) : logger(logger) {}

        void log(const std::string& message) { logger.log(message); }

        void log_payout(Values::PlayerId player_id, Values::Money amount) {
            logger.log(std::format("Выплата игроку ID {}: {}",
                                   player_id.get_id(), amount));
        }

        void log_balance(const Domain::Player& player) {
            logger.log(std::format("Обновление баланса игрока ID {}: {}",
                                   player.get_id().get_id(),
                                   player.get_balance()));
        }

        void log_bet(Values::PlayerId player_id, const Bets::Bet& bet) {
            logger.log(std::format("Ставка игрока ID {}: {}",
                                   player_id.get_id(), bet.describe()));
        }

        void log_exceptions(const std::string& error_message) {
            logger.log(std::format("[ОШИБКА] {}", error_message));
        }

        void log_newTable(const Aggregates::Table& table) {
            logger.log(std::format("Игровой стол создан с балансом: {}",
                                   table.get_balance()));
        }

        void log_newPlayer(const Domain::Player& player) {
            logger.log(std::format("Игрок ID {} вошел с балансом: {}",
                                   player.get_id().get_id(),
                                   player.get_balance()));
        }

        void log_spin_result(Values::RouletteNumber number) {
            std::string colorStr =
                (number.color() == Values::Color::Red)     ? "Красное"
                : (number.color() == Values::Color::Black) ? "Черное"
                                                           : "Зеленое (Зеро)";
            logger.log(std::format("Рулетка выдала число: {} ({})",
                                   number.value(), colorStr));
        }
    };
}  // namespace Infrastructure
