#pragma once
#include "TableRepository.hpp"

namespace Repository {

    class InMemoryTableRepository : public TableRepository {
        InMemoryTableRepository();
        std::optional<Table> InMemoryTableRepository::load(TableId id);
        void InMemoryTableRepository::save(const Table& table);
        ~InMemoryTableRepository();
    };

}  // namespace Repository
