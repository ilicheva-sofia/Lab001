#pragma once
#include <cstdint>

#include "DomainError.hpp"

namespace Values {
    class PlayerId {
        std::int64_t value;

       public:
        explicit PlayerId(std::int64_t value) : value(value) {
            if (value <= 0) throw Exceptions::InvalidPlayerId(value);
        }
        bool operator==(const PlayerId& other) const {
            return value == other.value;
        }
    };
}  // namespace Values