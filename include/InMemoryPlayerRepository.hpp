#pragma once
#include <map>
#include <memory>

#include "Player.hpp"
#include "PlayerId.hpp"
#include "PlayerRepository.hpp"

namespace Repository {

    class InMemoryPlayerRepository : public PlayerRepository {
       private:
        std::map<std::int64_t, std::shared_ptr<Domain::Player>> m_players;

       public:
        InMemoryPlayerRepository() = default;

        std::shared_ptr<Domain::Player> load(Values::PlayerId id) override {
            if (m_players.contains(id.get_id())) {
                return m_players.at(id.get_id());
            }
            return nullptr;  // Если не нашли, возвращаем пустой указатель
        }

        void save(std::shared_ptr<Domain::Player> player) override {
            if (player) {
                m_players.insert_or_assign(player->get_id().get_id(), player);
            }
        }
        ~InMemoryPlayerRepository() override = default;
    };
}  // namespace Repository
