#pragma once
#include <fstream>
#include <string>

#include "Logger.hpp"
/*
#include "Money.hpp"
#include "PlayerId.hpp"
 */
namespace Infrastructure {
    /*     using Bets::Bet;
        using Domain::Player;
        using Values::Money;
        using Values::PlayerId;

     */
    class FileLogger : public Ports::Logger {
       private:
        std::ofstream logFile;
        std::string filePath;

       public:
        explicit FileLogger(const std::string& path);
        // Деструктор для корректного закрытия файла
        ~FileLogger();

        void log(const std::string& message);
        /*
                void log_payout(PlayerId player_id, Money amount);
                void log_balance(Player player);
                void log_bet(PlayerId player_id, Bet& bet);
                void log_exceptions(std::string str);
          */
    };

}  // namespace Infrastructure