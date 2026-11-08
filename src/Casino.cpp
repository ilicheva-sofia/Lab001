#include <ctime>
#include <iostream>
#include <memory>

#include "BotStrategy.hpp"
#include "FileLogger.hpp"
#include "FilePlayerRepository.hpp"
#include "FileTableRepository.hpp"
#include "GameService.hpp"
#include "InMemoryLogger.hpp"
#include "InMemoryPlayerRepository.hpp"
#include "InMemoryTableRepository.hpp"
#include "LoggingService.hpp"
#include "RealRandomGenerator.hpp"
#include "Tablelimits.hpp"

void bot_play() {
    // new Random Generator (pure for  game,  fake for test)
    // 42 - seed
    auto rng = std::make_unique<Infrastructure::RealRandomGenerator>(7742);
    //    rng = std::make_unique<FakeGenerator>();

    // new logger :  memory, file, sql
    auto fileLogger = std::make_unique<Infrastructure::FileLogger>("game.log");
    auto loggingService =
        std::make_unique<Infrastructure::LoggingService>(*fileLogger);

    // InMemoryPlayerRepository, FilePlayerRepository, SQLightPlayerRepository
    auto playerRepo =
        std::make_unique<Repository::FilePlayerRepository>("player.txt");
    // FileTableRepository, SQLightTableRepository
    auto tableRepo =
        std::make_unique<Repository::FileTableRepository>("table.txt");

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
}

void human_play() {
    std::uint32_t seed_seconds = static_cast<std::uint32_t>(std::time(nullptr));
    auto rng =
        std::make_unique<Infrastructure::RealRandomGenerator>(seed_seconds);

    auto Logger = std::make_unique<Infrastructure::MemoryLogger>();
    auto loggingService =
        std::make_unique<Infrastructure::LoggingService>(*Logger);

    auto playerRepo = std::make_unique<Repository::InMemoryPlayerRepository>();

    auto tableRepo = std::make_unique<Repository::InMemoryTableRepository>();
    Domain::GameService gameService(*playerRepo, *tableRepo, *loggingService);

    Values::TableId tid(1);
    std::int64_t tmin, tmax;
    std::cout << "Введите лимит стола через пробел минимум и максимум :";

    std::cin >> tmin >> tmax;
    Values::Tablelimits limits{Values::Money(tmin * 100),
                               Values::Money(tmax * 100)};

    std::int64_t tamount;
    std::cout << "Введите кассу стола :";
    std::cin >> tamount;
    auto table = std::make_shared<Aggregates::Table>(
        tid, Values::Money(tamount * 100), limits);
    tableRepo->save(table);
    loggingService->log_newTable(*table);

    std::int64_t pamount;
    std::cout << "Введите баланс игрока :";
    std::cin >> pamount;

    Values::Money start_capital(pamount * 100);
    Values::PlayerId pid(1);
    auto player1 = std::make_shared<Domain::Player>(pid, start_capital);
    playerRepo->save(player1);
    loggingService->log_newPlayer(*player1);

    constexpr std::string_view color_set = "цЦcC";
    constexpr std::string_view number_set = "чЧnN";

    while (true) {
        std::cout << "Введите сумму ставки игрока :";
        std::cin >> pamount;

        if (pamount < 0) {
            std::cout << "Сумма ставки не может быть отрицательной."
                      << std::endl;
            continue;
        }

        Values::Money bet_amount{pamount * 100};
        if (!limits.validate(bet_amount)) {
            std::cout << std::format(
                "Ставка не принята. Допустимый диапазон: {} — {}\n",
                limits.get_min_limits(), limits.get_max_limits());
            continue;
        }

        std::cout << "Введите тип ставки игрока: цвет или число (" << color_set
                  << "/" << number_set << ")";
        std::string ans;
        std::cin >> ans;
        std::unique_ptr<Bets::Bet> bet;
        if (color_set.find(ans) != std::string::npos) {
            constexpr std::string_view red_set = "кКrR";
            constexpr std::string_view black_set = "чЧbB";
            std::cout << "Введите цвет (" << red_set << "/" << black_set
                      << ")";
            std::string cc;
            std::cin >> cc;
            if (red_set.find(cc) != std::string::npos) {
                bet = std::make_unique<Bets::ColorBet>(bet_amount,
                                                       Values::Color::Red);
            } else if (black_set.find(cc) != std::string::npos) {
                bet = std::make_unique<Bets::ColorBet>(bet_amount,
                                                       Values::Color::Black);
            } else {
                std::cout << "Недопустимый ответ. Выходим" << std::endl;
                return;
            }

        } else if (number_set.find(ans) != std::string::npos) {
            int digit;
            std::cout << "Введите число ( от 0 до 36 )";

            std::cin >> digit;
            if (!std::cin.good() || digit < 0 || digit > 36) {
                std::cout << "Недопустимое число. Выходим" << std::endl;
                return;
            }
            std::cout << "Введено число ( от 0 до 36 )" << digit << std::endl;

            bet = std::make_unique<Bets::SingleNumberBet>(
                bet_amount,
                Values::RouletteNumber{static_cast<std::int8_t>(digit)});

        } else {
            std::cout << "Недопустимый ответ. Выходим" << std::endl;
            return;
        }
        try {
            gameService.place_bet(pid, tid, std::move(bet));
        } catch (const Exceptions::InvalidNumber& e) {
            std::cout << "Недопустимая ставка. Выходим" << std::endl;
            return;
        }
        std::cout << "Крутим рулетку" << std::endl;

        Values::SpinResult result = gameService.spin(tid, *rng);
        std::string colorStr =
            (result.get_number().color() == Values::Color::Red)     ? "КРАСНОЕ"
            : (result.get_number().color() == Values::Color::Black) ? "ЧЕРНОЕ"
                                                                    : "ЗЕРО";
        std::cout << std::format("  Выпало число: {} ({})\n",
                                 result.get_number().value(), colorStr);
        std::cout << std::format(
            "  Текущий баланс игрока: {}\n",
            playerRepo->load(pid)->get_balance());

        if (result.get_status() == Values::SpinStatus::Cancelled) {
            std::cout << "Стол обанкротился!" << std::endl;
            return;
        }

        bool won = false;
        for (const auto& payout : result.get_payouts()) {
            if (payout.get_player().get_id() == pid.get_id() &&
                payout.get_amount() > Values::Money::zero()) {
                won = true;
                std::cout << std::format("ставка сыграла, выигрыш: {} \n",
                                         payout.get_amount());
                break;
            }
        }

        if (!won) {
            std::cout << "Игрок  проиграл!" << std::endl;
        }

        // Вопрос: играть ещё?
        constexpr std::string_view yes_set = "дДyY";
        std::string again;
        std::cout << "Сделать ещё ставку? (д/н) ";
        std::cin >> again;
        if (yes_set.find(again) == std::string::npos) {
            auto final_balance = playerRepo->load(pid)->get_balance();
            std::cout << std::format("Игра окончена. Итоговый баланс: {}\n",
                                     final_balance);
            return;
        }
    }
}

int main() {
    std::string ans;
    constexpr std::string_view set_of_bot_letters = "бБbB";
    constexpr std::string_view set_of_human_letters = "чЧhH";
    std::cout << "Кто будет играть:боты или человек? (введите символ) ("
              << set_of_bot_letters << "/" << set_of_human_letters << ") ";
    std::cin >> ans;
    if (set_of_bot_letters.find(ans) != std::string::npos) {
        bot_play();
    } else if (set_of_human_letters.find(ans) != std::string::npos) {
        human_play();
    } else {
        std::cout << "Недопустимый ответ. Выходим" << std::endl;
    }
    return 0;
}
