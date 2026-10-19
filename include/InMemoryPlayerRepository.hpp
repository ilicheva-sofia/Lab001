#pragma once
#include <map>
#include <optional>

#include "Player.hpp"
#include "PlayerRepository.hpp"

namespace Repository {

    class InMemoryPlayerRepository : public PlayerRepository {
       private:
        std::map<std::int64_t, Domain::Player> players;

       public:
        InMemoryPlayerRepository() = default;

        std::optional<Domain::Player> load(Values::PlayerId id) override {
            if (players.contains(id.get_id())) {
                return players.at(id.get_id());
            }
            return std::nullopt;
        }

        void save(const Domain::Player& table) override {
            players.insert_or_assign(table.get_id().get_id(), table);
        }

        ~InMemoryPlayerRepository() override = default;
    };

}  // namespace Repository
