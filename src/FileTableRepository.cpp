#include "FileTableRepository.hpp"

#include <fstream>
#include <map>
#include <memory>
#include <sstream>

namespace Repository {

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
        std::string line;
        while (std::getline(file, line)) {
            std::int64_t table_id = 0;
            std::int64_t table_min = 0;
            std::int64_t table_max = 0;
            std::int64_t kopecks = 0;
            std::stringstream ss(line);
            ss >> table_id&& ss.peek() == ',' && ss.ignore() &&
                ss >> table_min&& ss.peek() == ',' && ss.ignore() &&
                ss >> table_max&& ss.peek() == ',' && ss.ignore() &&
                ss >> kopecks;
            if (table_id == id.get_id()) {
                return std::make_shared<Aggregates::Table>(
                    Values::TableId(id), Values::Money(kopecks),
                    Values::Tablelimits(Values::Money(table_min),
                                        Values::Money(table_max)));
            }
        }
        return nullptr;  // Если не нашли, возвращаем пустой указатель
    }

    void FileTableRepository::save(std::shared_ptr<Aggregates::Table> table) {
        if (!table) {
            return;
        }
        struct TableData {
            std::int64_t table_min;
            std::int64_t table_max;
            std::int64_t kopecks;
        };

        std::map<std::int64_t, TableData> all_tables;
        std::ifstream file(filename);
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            std::int64_t table_id = 0;
            std::int64_t table_min = 0;
            std::int64_t table_max = 0;
            std::int64_t kopecks = 0;
            std::stringstream ss(line);
            ss >> table_id&& ss.peek() == ',' && ss.ignore() &&
                ss >> table_min&& ss.peek() == ',' && ss.ignore() &&
                ss >> table_max&& ss.peek() == ',' && ss.ignore() &&
                ss >> kopecks;
            TableData td(table_min, table_max, kopecks);
            all_tables.insert_or_assign(table_id, td);
        }
        TableData tdo(table->get_limits().get_min_limits().get_kopecks(),
                      table->get_limits().get_max_limits().get_kopecks(),
                      table->get_balance().get_kopecks());
        all_tables.insert_or_assign(table->get_id().get_id(), tdo);
        std::ofstream outfile(filename);
        for (const auto& [id, tdata] : all_tables) {
            outfile << id << "," << tdata.table_min << "," << tdata.table_max
                    << "," << tdata.kopecks << "\n";
        }
    }
}  // namespace Repository
