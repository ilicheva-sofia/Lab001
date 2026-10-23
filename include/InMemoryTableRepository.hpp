#pragma once
#include <map>

#include "TableRepository.hpp"

namespace Repository {

    class InMemoryTableRepository : public TableRepository {
       private:
        std::map<std::int64_t, std::shared_ptr<Aggregates::Table>> tables;

       public:
        InMemoryTableRepository() = default;

        std::shared_ptr<Aggregates::Table> load(Values::TableId id) override {
            if (tables.contains(id.get_id())) {
                return tables.at(id.get_id());
            }
            return nullptr;  // Если не нашли, возвращаем пустой указатель
        }

        void save(std::shared_ptr<Aggregates::Table> table) override {
            if (table) {
                tables.insert_or_assign(table->get_id().get_id(), table);
            }
        }

        ~InMemoryTableRepository() override = default;
    };

}  // namespace Repository
