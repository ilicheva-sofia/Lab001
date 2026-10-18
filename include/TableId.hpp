#pragma once
#include <cstdint>

#include "DomainError.hpp"

namespace Values {
    class TableId {
       private:
        std::int64_t id;

       public:
        explicit TableId(std::int64_t id) : id(id) {
            if (id <= 0) throw Exceptions::InvalidTableId(id);
        }

        std::int64_t get_id() const { return id; }

        bool operator==(const TableId& other) const { return id == other.id; }
    };
}  // namespace Values
