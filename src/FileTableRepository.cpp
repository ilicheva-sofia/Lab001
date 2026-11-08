#include "FileTableRepository.hpp"

#include <cstdint>
#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Repository {
    namespace {
        struct TableData {
            std::int64_t table_min;
            std::int64_t table_max;
            std::int64_t kopecks;
        };

        std::vector<std::string> fields(const std::string& line) {
            std::vector<std::string> result;
            std::stringstream stream(line);
            std::string field;
            while (std::getline(stream, field, ',')) result.push_back(field);
            return result;
        }
    }  // namespace

    FileTableRepository::FileTableRepository(std::string filename)
        : filename(filename) {
        std::ofstream touch_file(filename, std::ios::app);
        if (touch_file.is_open()) {
            touch_file.close();
        }
    }

    std::shared_ptr<Aggregates::Table> FileTableRepository::load(
        Values::TableId id) {
        std::ifstream file(filename);
        if (!file.is_open()) return nullptr;

        std::shared_ptr<Aggregates::Table> table;
        std::vector<std::vector<std::string>> saved_bets;
        std::string line;
        while (std::getline(file, line)) {
            auto f = fields(line);
            try {
                if (f.size() == 4 && f[0] != "B" &&
                    std::stoll(f[0]) == id.get_id()) {
                    table = std::make_shared<Aggregates::Table>(
                        id, Values::Money(std::stoll(f[3])),
                        Values::Tablelimits(Values::Money(std::stoll(f[1])),
                                            Values::Money(std::stoll(f[2]))));
                } else if (f.size() == 7 && f[0] == "B" &&
                           std::stoll(f[1]) == id.get_id()) {
                    saved_bets.push_back(std::move(f));
                }
            } catch (const std::exception&) {
                // Повреждённая строка не должна скрывать остальные столы.
            }
        }

        if (!table) return nullptr;
        for (const auto& f : saved_bets) {
            try {
                Values::PlayerId player(std::stoll(f[2]));
                Values::Money amount(std::stoll(f[4]));
                if (f[3] == "C") {
                    table->restore_bet(
                        player, std::make_unique<Bets::ColorBet>(
                                    amount, static_cast<Values::Color>(
                                                std::stoi(f[5]))));
                } else if (f[3] == "N") {
                    table->restore_bet(
                        player, std::make_unique<Bets::SingleNumberBet>(
                                    amount, Values::RouletteNumber(
                                                std::stoi(f[5]))));
                }
            } catch (const std::exception&) {
                // Пропускаем только некорректную ставку.
            }
        }
        return table;
    }

    void FileTableRepository::save(std::shared_ptr<Aggregates::Table> table) {
        if (!table) return;

        std::map<std::int64_t, TableData> all_tables;
        std::vector<std::string> other_bets;
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            auto f = fields(line);
            try {
                if (f.size() == 4 && f[0] != "B") {
                    all_tables.insert_or_assign(
                        std::stoll(f[0]),
                        TableData{std::stoll(f[1]), std::stoll(f[2]),
                                  std::stoll(f[3])});
                } else if (f.size() == 7 && f[0] == "B" &&
                           std::stoll(f[1]) != table->get_id().get_id()) {
                    other_bets.push_back(line);
                }
            } catch (const std::exception&) {
            }
        }

        all_tables.insert_or_assign(
            table->get_id().get_id(),
            TableData{table->get_limits().get_min_limits().get_kopecks(),
                      table->get_limits().get_max_limits().get_kopecks(),
                      table->get_balance().get_kopecks()});

        std::ofstream outfile(filename);
        for (const auto& [id, data] : all_tables) {
            outfile << id << ',' << data.table_min << ',' << data.table_max
                    << ',' << data.kopecks << '\n';
        }
        for (const auto& saved : other_bets) outfile << saved << '\n';

        for (const auto& placed : table->get_bets()) {
            if (const auto* bet =
                    dynamic_cast<const Bets::ColorBet*>(placed.bet.get())) {
                outfile << "B," << table->get_id().get_id() << ','
                        << placed.player.get_id() << ",C,"
                        << bet->get_amount().get_kopecks() << ','
                        << static_cast<int>(bet->get_color()) << ",0\n";
            } else if (const auto* bet = dynamic_cast<const Bets::SingleNumberBet*>(
                           placed.bet.get())) {
                outfile << "B," << table->get_id().get_id() << ','
                        << placed.player.get_id() << ",N,"
                        << bet->get_amount().get_kopecks() << ','
                        << static_cast<int>(bet->get_number().value()) << ",0\n";
            }
        }
    }
}  // namespace Repository
