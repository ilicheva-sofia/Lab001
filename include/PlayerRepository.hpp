#pragma once
#include <optional>

#include "Player.hpp"
#include "PlayerId.hpp"

namespace Repository {

    using Domain::Player;
    using Values::PlayerId;

    struct PlayerRepository {
        virtual std::optional<Player> load(PlayerId id) = 0;
        virtual void save(const Player& player) = 0;
        virtual ~PlayerRepository() = default;
    };

    class InMemoryPlayerRepository : public PlayerRepository {
        // реализация сохранения в памяти
    };

    class FilePlayerRepository : public PlayerRepository {
        // реализация сохранения в файл
    };

    class SQLightPlayerRepository : public PlayerRepository {
        // реализация сохранения в файл
    };

}  // namespace Repository
