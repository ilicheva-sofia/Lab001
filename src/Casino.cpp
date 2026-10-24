#include <iostream>
#include <memory>

#include "BotStrategy.hpp"
#include "FileLogger.hpp"
#include "GameService.hpp"
#include "InMemoryPlayerRepository.hpp"
#include "InMemoryTableRepository.hpp"
#include "LoggingService.hpp"
#include "RealRandomGenerator.hpp"
#include "Tablelimits.hpp"

int main() {
    // new Random Generator (pure for  game,  fake for test)
    // 42 - seed
    auto rng = std::make_unique<Infrastructure::RealRandomGenerator>(42);
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

    Values::Money start_capital(10000'00);

    auto player1 =
        std::make_shared<Domain::Player>(Values::PlayerId(1), start_capital);
    playerRepo->save(player1);
    loggingService->log_newPlayer(*player1);

    auto player2 =
        std::make_shared<Domain::Player>(Values::PlayerId(2), start_capital);
    playerRepo->save(player2);
    loggingService->log_newPlayer(*player2);

    std::vector<std::shared_ptr<Domain::BotStrategy>> bots;
    bots.push_back(std::make_shared<Domain::RedConstantBot>(
        "Осторожный-Ред", Values::Money(5000'00), Values::Money(3000'00),
        start_capital, Values::Money(200'00)));

    bots.push_back(std::make_shared<Domain::MartingaleBot>(
        "Мартингейл-Блэк", Values::Money(5000'00), Values::Money(5000'00),
        start_capital, Values::Money(100'00)));

    Domain::GameService gameService(*playerRepo, *tableRepo, *loggingService);

    gameService.startGame(bots, tid, *rng, 100);

    std::cout << "Результаты успешно сохранены в файл game.log\n";
    return 0;
}
