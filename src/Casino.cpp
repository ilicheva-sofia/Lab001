
#include <memory>

#include "PlayerRepository.hpp"
#include "RealRandomGenerator.hpp"

void main(void) {
    // new Random Generator (pure for  game,  fake for test)
    auto rng = std::make_unique<Infrastructure::RealRandomGenerator>();
    //    rng = std::make_unique<FakeGenerator>();

    // new logger :  memory, file, sql
    // 2. Создаем логгер
    std::unique_ptr<Logger> logger = std::make_unique<FileLogger>("game.log");

    // 3. Создаем репозитории
    // new  player's  repo PlayerRepository   which one
    // InMemoryPlayerRepository, FilePlayerRepository, SQLightPlayerRepository

    std::unique_ptr<PlayerRepository> playerRepo =
        std::make_unique<InMemoryPlayerRepository>();

    // new  table's  repo TableRepository .. which one InMemoryTableRepository,
    // FileTableRepository, SQLightTableRepository

    std::unique_ptr<TableRepository> tableRepo =
        std::make_unique<InMemoryTableRepository>();

    // 4. Создаем игровые столы
    // new table ,  what is the  balance of table  // log  balance
    // add  table to table repo  , how many tables  // log ?? what ??

    Table table(10000);  // начальный баланс стола
    tableRepo->add(table);
    logger->log("Table created with balance: " +
                std::to_string(table.getBalance()));

    // 5. Создаем игроков
    // new player , what is the balance of player  //  log  balance
    // add player  to player repo , how many players //  log ??

    Player player(1000);  // начальный баланс игрока
    playerRepo->add(player);
    logger->log("Player created with balance: " +
                std::to_string(player.getBalance()));

    // 6. Создаем GameService со всеми зависимостями
    GameService gameService(*rng, *logger, *playerRepo, *tableRepo);

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
