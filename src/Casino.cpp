
#include <memory>

#include "FileLogger.hpp"
#include "InMemoryPlayerRepository.hpp"
#include "LoggingService.hpp"
#include "PlayerRepository.hpp"
#include "RealRandomGenerator.hpp"

void main(void) {
    // new Random Generator (pure for  game,  fake for test)
    auto rng = std::make_unique<Infrastructure::RealRandomGenerator>();
    //    rng = std::make_unique<FakeGenerator>();

    // new logger :  memory, file, sql
    std::unique_ptr<Logger> logger = std::make_unique<FileLogger>("game.log");
    Infrastructure::LoggingService loggingService(*fileLogger);

    // new  player's  repo PlayerRepository   which one
    // InMemoryPlayerRepository, FilePlayerRepository, SQLightPlayerRepository

    std::unique_ptr<PlayerRepository> playerRepo =
        std::make_unique<InMemoryPlayerRepository>();

    // new  table's  repo TableRepository .. which one InMemoryTableRepository,
    // FileTableRepository, SQLightTableRepository

    std::unique_ptr<TableRepository> tableRepo =
        std::make_unique<InMemoryTableRepository>();

    // new table ,  what is the  balance of table  // log  balance
    // add  table to table repo  , how many tables  // log ?? what ??

    Table table(10000);  // начальный баланс стола
    tableRepo->add(table);
    loggingService->log_newTable(table);

    // new player , what is the balance of player  //  log  balance
    // add player  to player repo , how many players //  log ??

    Player player(2000);  // начальный баланс игрока
    playerRepo->add(player);
    loggingService->log_newPlayer(player);

    GameService gameService(*rng, *loggingService, *playerRepo, *tableRepo);

    player->place_bet(ColorBet{Color::Red});
    table.accept_bet(bet);

    // player[i] makes new  bet :  numer or color, etc
    // player[i] place bet to table[j]
    // Одно вращение — один результат для всех
    // table [j] accept bet -  check table  limits exception   // log  bet
    // spin result  = table [j].spin(& randomGenerator)  -  log  spin  result
    // for each player on table[j]  check  bet  with SpinResult  - where ??
    //       player balance ++  table balance --     //  log  balance

    // 7. Начинаем игру
    gameService.startGame();
}
