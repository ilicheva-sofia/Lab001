#pragma once
#include <format>
#include <string>

#include "Bet.hpp"
#include "Logger.hpp"
#include "Money.hpp"
#include "PayoutEntry.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"

namespace Infrastructure {
    using Bets::Bet;
    using Domain::Player;
    using Ports::Logger;
    using Values::Money;
    using Values::PlayerId;

    class LoggingService {
       private:
        Logger& logger;

       public:
        explicit LoggingService(Logger& logger) : logger(logger) {}

        void log_payout(PlayerId player_id, Money amount) {
            logger.log(std::format("выплата игроку: {} в количестве: {}",
                                   player_id, amount));
        }

        void log_balance(const Player& player) {
            logger.log(
                std::format("обновление баланса игрока: {} новый баланс: {}",
                            player.get_id(), player.balance()));
        }

        void log_bet(PlayerId player_id, const Bet& bet) {
            logger.log(std::format("ставка игрока: {} сумма: {} тип: {}",
                                   player_id, bet.get_amount(),
                                   bet.get_type()));
        }

        void log_exceptions(const std::string& error_message) {
            logger.log(std::format("ошибка: {}", error_message));
        }
        void log_newTable(const Table& table) {
            logger->log("Table created with balance: " +
                        std::to_string(table.balance()));
        }
        void log_newPlayer(const Player& player) {
            logger->log("Player created with balance: " +
                        std::to_string(player.balance()));
        };

    }  // namespace Infrastructure
