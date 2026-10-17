#pragma once

// #include ""
#include "PlayerRepository.hpp"
#include "RandomGenerator.hpp"
#include "SpinResult.hpp"
#include "Table.hpp"
#include "TableRepository.hpp"

namespace Domain {
    using Aggregates::Table;
    using Bets::Bet;
    using Ports::RandomGenerator;
    using Repository::PlayerRepository;
    using Repository::TableRepository;
    using Values::SpinResult;
    using Values::TableId;

    class GameService {
        PlayerRepository& players;
        TableRepository& tables;
        //        Logger& logger;

       public:
        // Фаза 1: приём ставок — можно вызвать много раз для разных игроков
        void place_bet(PlayerId pid, TableId tid, std::unique_ptr<Bet> bet);

        // Фаза 2: вращение — один раз за раунд, считает все ставки сразу
        SpinResult spin(TableId tid, RandomGenerator& rng);
        void save_table(TableId tid);
        Table load_table(TableId tid);
    };

}  // namespace Domain