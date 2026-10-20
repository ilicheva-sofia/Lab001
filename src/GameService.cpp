#include "GameService.hpp"

#include <format>

#include "DomainError.hpp"

namespace Domain {

    Values::SpinResult GameService::spin(Values::TableId tid,
                                         Ports::RandomGenerator& rng) {
        auto table_opt = tables.load(tid);
        if (!table_opt) {
            logger.log_exceptions("Стол для прокрутки рулетки не найден!");
            throw std::runtime_error("Table not found");
        }

        Aggregates::Table& table = table_opt.value();

        Values::SpinResult result = table.spin(rng);
        logger.log_spin_result(result.get_number());

        if (result.get_status() == Values::SpinStatus::Completed) {
            for (const auto& entry : result.get_payouts()) {
                auto player_opt = players.load(entry.player);
                if (player_opt) {
                    Domain::Player& player = player_opt.value();
                    player.receive_payout(entry.amount);
                    players.save(player);

                    logger.log_payout(entry.player, entry.amount);
                    logger.log_balance(player);
                }
            }
        } else {
            logger.log_exceptions(
                "Вращение отменено из-за форс-мажора (банкротство стола). "
                "Ставки аннулированы.");
        }

        tables.save(table);
        return result;
    }

    void GameService::place_bet(Values::PlayerId pid, Values::TableId tid,
                                std::unique_ptr<Bets::Bet> bet) {
        auto player_opt = players.load(pid);
        auto table_opt = tables.load(tid);

        if (!player_opt || !table_opt) {
            logger.log_exceptions("Игрок или Стол не найдены в системе!");
            return;
        }

        Domain::Player& player = player_opt.value();
        Aggregates::Table& table = table_opt.value();

        try {
            player.spend_money(bet->get_amount());
            logger.log_bet(pid, *bet);
            table.accept_bet(pid, std::move(bet));

            players.save(player);
            tables.save(table);

        } catch (const Exceptions::InvalidMoneyOperation& e) {
            logger.log_exceptions(std::format("Ошибка списание у игрока {}: {}",
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
