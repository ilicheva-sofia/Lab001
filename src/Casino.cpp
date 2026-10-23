#include <iostream>
#include <memory>

#include "FileLogger.hpp"
#include "GameService.hpp"
#include "InMemoryPlayerRepository.hpp"
#include "InMemoryTableRepository.hpp"
#include "LoggingService.hpp"
#include "RealRandomGenerator.hpp"
#include "Tablelimits.hpp"

int main(void) {
    // new Random Generator (pure for  game,  fake for test)
    auto rng = std::make_unique<Infrastructure::RealRandomGenerator>();
    //    rng = std::make_unique<FakeGenerator>();

    // new logger :  memory, file, sql
    auto fileLogger = std::make_unique<Infrastructure::FileLogger>("game.log");
    auto loggingService =
        std::make_unique<Infrastructure::LoggingService>(*fileLogger);

    // InMemoryPlayerRepository, FilePlayerRepository, SQLightPlayerRepository
    auto playerRepo = std::make_unique<Repository::InMemoryPlayerRepository>();
    // FileTableRepository, SQLightTableRepository
    auto tableRepo = std::make_unique<Repository::InMemoryTableRepository>();

    Values::TableId tid(1);
    Values::Tablelimits limits(Values::Money(100'00), Values::Money(10000'00));

    auto table = std::make_shared<Aggregates::Table>(
        tid, Values::Money(100000'00), limits);
    tableRepo->save(table);
    loggingService->log_newTable(*table);

    Values::PlayerId pid(1);
    auto player = std::make_shared<Domain::Player>(pid, Values::Money(5000'00));
    playerRepo->save(player);
    loggingService->log_newPlayer(*player);

    Domain::GameService gameService(*playerRepo, *tableRepo, *loggingService);
    // player[i] makes new  bet :  numer or color, etc
    // player[i] place bet to table[j]
    // Одно вращение — один результат для всех
    // table [j] accept bet -  check table  limits exception   // log  bet
    // spin result  = table [j].spin(& randomGenerator)  -  log  spin  result
    // for each player on table[j]  check  bet  with SpinResult  - where ??
    //       player balance ++  table balance --     //  log  balance
    gameService.startGame();

    std::cout << "--- РАУНД 1: Размещение ставок ---\n";
    gameService.place_bet(pid, tid,
                          std::make_unique<Bets::ColorBet>(
                              Values::Money(1000'00), Values::Color::Red));

    std::cout << "--- РАУНД 1: Вращение рулетки ---\n";
    gameService.spin(tid, *rng);

    std::cout
        << "Симуляция успешно завершена! Результаты записаны в файл game.log\n";
    return 0;
}
