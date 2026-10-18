#pragma once
#include <memory>
#include <optional>

#include "Bet.hpp"
#include "LoggingService.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
#include "SpinResult.hpp"
#include "Table.hpp"

namespace Domain {
    class GameService {
       private:
        Repository::PlayerRepository& players;
        Repository::TableRepository& tables;
        Infrastructure::LoggingService& logger;

       public:
        GameService(Repository::PlayerRepository& p,
                    Repository::TableRepository& t,
                    Infrastructure::LoggingService& l)
            : players(p), tables(t), logger(l) {}

        void place_bet(Values::PlayerId pid, Values::TableId tid,
                       std::unique_ptr<Bets::Bet> bet);
        Values::SpinResult spin(Values::TableId tid,
                                Ports::RandomGenerator& rng);

        void startGame();
    };
}  // namespace Domain
