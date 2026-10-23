#pragma once
#include <memory>

#include "Table.hpp"
#include "TableId.hpp"

namespace Repository {

    struct TableRepository {
        virtual std::shared_ptr<Aggregates::Table> load(Values::TableId id) = 0;
        virtual void save(std::shared_ptr<Aggregates::Table> table) = 0;
        virtual ~TableRepository() = default;
    };

    class FileTableRepository : public TableRepository {
        // реализация сохранения в файл
    };

    class SQLightTableRepository : public TableRepository {
        // реализация сохранения в БД
    };

}  // namespace Repository
