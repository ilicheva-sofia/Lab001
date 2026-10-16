#pragma once
#include <optional>

#include "Table.hpp"
#include "TableId.hpp"

namespace Repository {
    using Aggregates::Table;
    using Values::TableId;

    struct TableRepository {
        virtual std::optional<Table> load(TableId id) = 0;
        virtual void save(const Table& table) = 0;
        virtual ~TableRepository() = default;
    };

    class InMemoryTableRepository : public TableRepository {
        // реализация сохранения в памяти
    };

    class FileTableRepository : public TableRepository {
        // реализация сохранения в файл
    };

    class SQLightTableRepository : public TableRepository {
        // реализация сохранения в файл
    };

}  // namespace Repository
