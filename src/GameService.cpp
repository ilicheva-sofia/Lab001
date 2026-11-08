#include "GameService.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <map>
#include <vector>

#include "DomainError.hpp"
#include "SvgChart.hpp"

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

    void GameService::record_balances(
        Values::TableId tid, std::size_t bot_count,
        std::map<std::string, std::vector<double>>& balances) {
        auto table = tables.load(tid);
        if (table) {
            balances["__table__"].push_back(
                static_cast<double>(table->get_balance().get_kopecks()) /
                100.0);
        }

        for (std::size_t i = 0; i < bot_count; ++i) {
            Values::PlayerId pid(static_cast<std::int64_t>(i + 1));
            auto player = players.load(pid);
            auto& series = balances["bot_" + std::to_string(i + 1)];
            double value =
                player
                    ? static_cast<double>(player->get_balance().get_kopecks()) /
                          100.0
                    : (series.empty() ? 0.0 : series.back());
            series.push_back(value);
        }
    }

    void GameService::startGame(
        std::vector<std::shared_ptr<Domain::BotStrategy>>& bots,
        Values::TableId tid, Ports::RandomGenerator& rng, int max_rounds) {
        logger.log(std::format("Запуск симуляции казино для {} ботов...\n",
                               bots.size()));

        std::map<std::string, std::vector<double>> balances;
        balances["__table__"] = {};

        static const std::vector<std::string> palette = {
            "#e41a1c", "#555555"};
        std::vector<std::string> bot_colors;
        for (size_t i = 0; i < bots.size(); ++i) {
            std::string key = "bot_" + std::to_string(i + 1);
            bot_colors.push_back(palette[i % palette.size()]);
            balances[key] = {};
        }
        // Заполняем начальные значения
        record_balances(tid, bots.size(), balances);

        // раунды
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

            // Фиксируем балансы после раунда
            record_balances(tid, bots.size(), balances);

            // Проверка на глобальный форс-мажор банкротства стола
            if (result.get_status() == Values::SpinStatus::Cancelled) {
                std::cout << "[КРИТ] Симуляция аварийно остановлена: Стол "
                             "обанкротился!\n";
                break;
            }
        }

        // Генерируем SVG-график балансов
        std::vector<Infrastructure::Series> series;
        // Стол
        series.push_back(
            {"Касса стола", "#4daf4a", balances["__table__"]});
        // Боты
        for (size_t i = 0; i < bots.size(); ++i) {
            series.push_back({bots[i]->get_name(), bot_colors[i],
                                balances["bot_" + std::to_string(i + 1)]});
        }
        Infrastructure::SvgChart::write(
            "balances_chart.svg", "Балансы ботов и стола по раундам",
            series);
        std::cout << "График сохранён в balances_chart.svg\n";

        logger.log("=== СИМУЛЯЦИЯ ИГРЫ УСПЕШНО ЗАВЕРШЕНА ===");
    }

}  // namespace Domain
