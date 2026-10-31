#include "GameService.hpp"

#include <format>
#include <iostream>

#include "DomainError.hpp"

namespace Domain {

    Values::SpinResult GameService::spin(Values::TableId tid,
                                         Ports::RandomGenerator& rng) {
        auto table = tables.load(tid);
        if (!table) {
            logger.log_exceptions("Стол для прокрутки рулетки не найден!");
            throw Exceptions::DomainError("Table not found");
        }

        Values::SpinResult result = table->spin(rng);
        logger.log_spin_result(result.get_number());

        if (result.get_status() == Values::SpinStatus::Completed) {
            for (const auto& entry : result.get_payouts()) {
                auto player = players.load(entry.get_player());
                if (!player) continue;

                if (entry.get_amount() > Values::Money::zero()) {
                    player->receive_payout(entry.get_amount());
                    players.save(player);
                    logger.log_payout(entry.get_player(), entry.get_amount());
                } else {
                    logger.log(std::format("Игрок ID {} проиграл ставку.",
                                           entry.get_player().get_id()));
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
                auto player = players.load(entry.get_player());
                if (!player) continue;

                player->receive_payout(entry.get_amount());
                players.save(player);

                logger.log(std::format(
                    "[ВОЗВРАТ] Ставка {} возвращена на счет игрока ID {}.",
                    entry.get_amount(), entry.get_player().get_id()));
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

        Values::Money bet_amount = bet->get_amount();
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
            player->receive_payout(bet_amount);
            players.save(player);
        }
        logger.log_balance(*player);
        logger.log(std::format("Текущая касса стола ID {}: {}", tid.get_id(),
                               table->get_balance()));
    }

    void GameService::startGame(
        std::vector<std::shared_ptr<Domain::BotStrategy>>& bots,
        Values::TableId tid, Ports::RandomGenerator& rng, int max_rounds) {
        logger.log(std::format("Запуск симуляции казино для {} ботов...\n",
                               bots.size()));

        for (int round = 1; round <= max_rounds; ++round) {
            logger.log(std::format("--- РАУНД {:02} ---", round));

            bool anyone_wants_to_play = false;

            // Фаза 1: Опрос всех ботов и сбор ставок
            for (size_t i = 0; i < bots.size(); ++i) {
                // Формируем PlayerId на основе индекса бота (1, 2, 3...)
                Values::PlayerId pid(static_cast<std::int64_t>(i + 1));
                auto player_live = players.load(pid);

                if (!player_live) continue;

                // Запрашиваем решение у бота
                auto table_live = tables.load(tid);
                Domain::BotDecision decision =
                    bots[i]->make_decision(*player_live);

                if (decision.wants_to_play) {
                    anyone_wants_to_play = true;
                    // Размещаем ставку через наш стандартный метод
                    place_bet(pid, tid, std::move(decision.bet));
                } else {
                    logger.log(std::format("   > Бот {} покинул стол.\n",
                                           bots[i]->get_name()));
                }
            }

            if (!anyone_wants_to_play) {
                logger.log(
                    "Все боты покинули казино. Остановка игрового цикла.");
                break;
            }

            Values::SpinResult result = spin(tid, rng);

            std::string colorStr =
                (result.get_number().color() == Values::Color::Red) ? "КРАСНОЕ"
                : (result.get_number().color() == Values::Color::Black)
                    ? "ЧЕРНОЕ"
                    : "ЗЕРО";
            std::cout << std::format("   Выпало число: {} ({})\n",
                                     result.get_number().value(), colorStr);

            for (size_t i = 0; i < bots.size(); ++i) {
                Values::PlayerId pid(static_cast<std::int64_t>(i + 1));
                bots[i]->update_result(result, pid);
            }

            // Вывод текущих балансов в консоль
            auto table_live = tables.load(tid);
            std::cout << std::format("   Касса казино: {}\n",
                                     table_live->get_balance());
            for (size_t i = 0; i < bots.size(); ++i) {
                Values::PlayerId pid(static_cast<std::int64_t>(i + 1));
                std::cout << std::format("   Баланс Бот {} ({}): {}\n", i + 1,
                                         bots[i]->get_name(),
                                         players.load(pid)->get_balance());
            }
            std::cout << "\n";

            // Проверка на глобальный форс-мажор банкротства стола
            if (result.get_status() == Values::SpinStatus::Cancelled) {
                std::cout << "[КРИТ] Симуляция аварийно остановлена: Стол "
                             "обанкротился!\n";
                break;
            }
        }

        logger.log("=== СИМУЛЯЦИЯ ИГРЫ УСПЕШНО ЗАВЕРШЕНА ===");
    }

}  // namespace Domain
