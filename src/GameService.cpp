#include "GameService.hpp"

#include <format>

#include "DomainError.hpp"

namespace Domain {

    Values::SpinResult GameService::spin(Values::TableId tid,
                                         Ports::RandomGenerator& rng) {
        auto table = tables.load(tid);
        if (!table) {
            logger.log_exceptions("Стол для прокрутки рулетки не найден!");
            throw std::runtime_error("Table not found");
        }

        Values::SpinResult result = table->spin(rng);
        logger.log_spin_result(result.get_number());

        if (result.get_status() == Values::SpinStatus::Completed) {
            for (const auto& entry : result.get_payouts()) {
                auto player = players.load(entry.player);
                if (!player) continue;

                if (entry.amount > Values::Money::zero()) {
                    player->receive_payout(entry.amount);
                    players.save(player);
                    logger.log_payout(entry.player, entry.amount);
                } else {
                    logger.log(std::format("Игрок ID {} проиграл ставку.",
                                           entry.player.get_id()));
                }

                logger.log_balance(*player);
            }

            logger.log(std::format("Касса стола после раунда: {}",
                                   table->get_balance()));

        } else {
            logger.log_exceptions(
                "Вращение отменено! Баланс стола недостаточен для выплат. "
                "Запущен возврат ставок.");

            for (const auto& entry : result.get_payouts()) {
                auto player = players.load(entry.player);
                if (!player) continue;

                player->receive_payout(entry.amount);
                players.save(player);

                logger.log(std::format(
                    "[ВОЗВРАТ] Ставка {} возвращена на счет игрока ID {}.",
                    entry.amount, entry.player.get_id()));
                logger.log_balance(*player);
            }
            logger.log(std::format("Касса стола после возврата ставок: {}",
                                   table->get_balance()));
        }

        tables.save(table);
        return result;
    }

    void GameService::place_bet(Values::PlayerId pid, Values::TableId tid,
                                std::unique_ptr<Bets::Bet> bet) {
        auto player = players.load(pid);
        auto table = tables.load(tid);

        if (!player || !table) {
            logger.log_exceptions("Игрок или Стол не найдены в системе!");
            return;
        }

        try {
            player->spend_money(bet->get_amount());
            logger.log_bet(pid, *bet);
            table->accept_bet(pid, std::move(bet));

            players.save(player);
            tables.save(table);

        } catch (const Exceptions::InvalidMoneyOperation& e) {
            logger.log_exceptions(std::format("Ошибка списания у игрока {}: {}",
                                              pid.get_id(), e.what()));
        } catch (const Exceptions::BetOutsideLimits& e) {
            logger.log_exceptions(
                std::format("Ставка не принята столом: {}", e.what()));
        }
    }

    void GameService::startGame() {
        logger.log("Сервис управления игрой успешно инициализирован.");
    }
}  // namespace Domain
