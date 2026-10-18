
#include "GameService.hpp"
namespace Domain {
    Table GameService::load_table(TableId tid) {
        return tables.load(tid).value();
    }
    void GameService::save_table(TableId tid) {
        Table table = tables.load(tid).value();
        // доменная логика
        tables.save(table);
    }

    SpinResult GameService::spin(TableId tid, RandomGenerator& rng) {
        Table table = tables.load(tid).value();

        SpinResult result = table.spin(rng);  // одно вращение, все ставки сразу

        for (const auto& entry : result.payouts()) {
            Player player = players.load(entry.player).value();
            player.receive_payout(entry.amount);
            players.save(player);
            logger.log_payout(entry.player, entry.amount);
            logger.log_balance(entry.player, player.balance());
        }

        tables.save(table);
        logger.log_spin(result);
        return result;
    }

    void GameService::place_bet(PlayerId player_id, TableId tid,
                                std::unique_ptr<Bet>& bet) {
        Player player = players.load(player_id).value();
        Table table = tables.load(tid).value();

        try {
            player.place_bet(bet);  // снять деньги у игрока
            logger->log_bet(player_id, const Bet& bet) {}
            catch (const Exceptions::InsufficientFundsException& e) {
                logger->log_exceptions("Недостаточно средств у игрока");
                try {
                    table.accept_bet(
                        pid, std::move(bet));  // передать ставку на стол
                } catch (const Exceptions::InvalidAmountException& e) {
                    logger->log_exceptions("Некорректная сумма ставки");
                }

                players.save(player);
                tables.save(table);
                logger.log_bet(pid, bet);
            }

            void GameService::startGame() {}

        }  // namespace Domain
