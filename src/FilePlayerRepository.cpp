#include "FilePlayerRepository.hpp"

#include <fstream>
#include <map>
#include <memory>
#include <sstream>

namespace Repository {

    FilePlayerRepository::FilePlayerRepository(std::string filename)
        : filename(filename) {
        std::ofstream touch_file(filename, std::ios::app);
        if (touch_file.is_open()) {
            touch_file.close();
        }
    }

    std::shared_ptr<Domain::Player> FilePlayerRepository::load(
        Values::PlayerId id) {
        std::ifstream file(filename);
        if (!file.is_open()) return nullptr;
        std::string line;
        while (std::getline(file, line)) {
            std::int64_t player_id = 0;
            std::int64_t kopecks = 0;
            std::stringstream ss(line);
            ss >> player_id&& ss.peek() == ',' && ss.ignore() && ss >> kopecks;
            if (player_id == id.get_id()) {
                return std::make_shared<Domain::Player>(Values::PlayerId(id),
                                                        Values::Money(kopecks));
            }
        }
        return nullptr;  // Если не нашли, возвращаем пустой указатель
    }

    void FilePlayerRepository::save(std::shared_ptr<Domain::Player> player) {
        if (!player) {
            return;
        }
        std::map<std::int64_t, std::int64_t> all_players;
        std::ifstream file(filename);
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            std::int64_t player_id = 0;
            std::int64_t kopecks = 0;
            std::stringstream ss(line);
            ss >> player_id&& ss.peek() == ',' && ss.ignore() && ss >> kopecks;
            all_players.insert_or_assign(player_id, kopecks);
        }
        all_players.insert_or_assign(player->get_id().get_id(),
                                     player->get_balance().get_kopecks());
        std::ofstream outfile(filename);
        for (const auto& [id, kop] : all_players) {
            outfile << id << "," << kop << "\n";
        }
    }
}  // namespace Repository
