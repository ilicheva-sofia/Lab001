#pragma once
#include <memory>

#include "Player.hpp"
#include "PlayerId.hpp"

namespace Repository {

    struct PlayerRepository {
        virtual std::shared_ptr<Domain::Player> load(Values::PlayerId id) = 0;
        virtual void save(std::shared_ptr<Domain::Player> player) = 0;
        virtual ~PlayerRepository() = default;
    };

    class FilePlayerRepository : public PlayerRepository {
        // реализация сохранения в файл
    };

    class SQLightPlayerRepository : public PlayerRepository {
        // реализация сохранения в БД
    };

}  // namespace Repository
