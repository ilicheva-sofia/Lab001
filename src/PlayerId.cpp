#include "PlayerId.hpp"

#include "DomainError.hpp"

namespace Values {
    PlayerId::PlayerId(std::int64_t id) : id(id) {
        if (id <= 0) {
            throw Exceptions::InvalidPlayerId(id);
        }
    }
}  // namespace Values
