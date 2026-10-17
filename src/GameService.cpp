
#include "GameService.hpp"

Table GameService::load_table(TableId tid) { return tables.load(tid).value(); }

void GameService::save_table(TableId tid) {
    Table table = tables.load(tid).value();
    // доменная логика
    tables.save(table);
}

SpinResult GameService::spin(TableId tid, RandomGenerator& rng) {
    Table table = tables_.load(tid).value();

    SpinResult result = table.spin(rng);  // одно вращение, все ставки сразу

    // Выплаты — по каждому игроку в результате
    for (const auto& entry : result.payouts()) {
        Player player = players.load(entry.player).value();
        player.deposit(entry.amount);
        players.save(player);
        logger.log_payout(entry.player, entry.amount);
        logger.log_balance(entry.player, player.balance());
    }

    tables.save(table);
    logger.log_spin(result);
    return result;
}

void GameService::place_bet(PlayerId pid, TableId tid,
                            std::unique_ptr<Bet> bet) {
    Player player = players.load(pid).value();
    RouletteTable table = tables.load(tid).value();

    player.withdraw(bet->amount());         // снять деньги у игрока
    table.accept_bet(pid, std::move(bet));  // передать ставку на стол

    players.save(player);
    tables.save(table);
    logger.log_bet(pid, *bet);
}
