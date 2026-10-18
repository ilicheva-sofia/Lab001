#pragma once
#include <map>
#include <optional>

#include "Table.hpp"
#include "TableRepository.hpp"

namespace Repository {

    class InMemoryTableRepository : public TableRepository {
       private:
        std::map<std::int64_t, Aggregates::Table> tables;

       public:
        InMemoryTableRepository() = default;

        std::optional<Aggregates::Table> load(Values::TableId id) override {
            if (tables.contains(id.get_id())) {
                return tables.at(id.get_id());
            }
            return std::nullopt;
        }

        void save(const Aggregates::Table& table) override {
            tables.insert_or_assign(table.get_id().get_id(), table);
        }

        ~InMemoryTableRepository() override = default;
    };

}  // namespace Repository
