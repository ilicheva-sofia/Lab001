#pragma once
#include <cstdint>

namespace Values {
    class PlayerId {
       private:
        std::int64_t id;

       public:
        explicit PlayerId(std::int64_t id);

        std::int64_t get_id() const { return id; }

        bool operator==(const PlayerId& other) const { return id == other.id; }
        bool operator<(const PlayerId& other) const {
            return id < other.id;
        }  // Нужно для std::map/set
    };
}  // namespace Values
