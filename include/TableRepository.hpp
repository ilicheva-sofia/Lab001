#pragma once
#include <map>
#include <optional>

#include "Table.hpp"
#include "TableId.hpp"

namespace Repository {

    struct TableRepository {
        virtual std::optional<Aggregates::Table> load(Values::TableId id) = 0;
        virtual void save(const Aggregates::Table& table) = 0;
        virtual ~TableRepository() = default;
    };

        class FileTableRepository : public TableRepository {
        // реализация сохранения в файл
    };

    class SQLightTableRepository : public TableRepository {
        // реализация сохранения в БД
    };

}  // namespace Repository
