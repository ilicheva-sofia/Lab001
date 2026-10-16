#pragma once
#include <cstdint>

#include "DomainError.hpp"

namespace Values {
    class TableId {
        std::int64_t value;

        TableId(std::int64_t value) : value(value) {
            if (value <= 0) throw Exceptions::InvalidTableId(value);
        }
    };
}  // namespace Values