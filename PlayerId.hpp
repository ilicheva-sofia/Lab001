#pragma once
#include <cstdint>

#include "DomainError.hpp"

namespace Values {
    class PlayerId {
        std::int64_t value;

        PlayerId(std::int64_t value) : value(value) {
            if (value <= 0) throw Exceptions::InvalidPlayerId(value);
        }
    };
}  // namespace Values