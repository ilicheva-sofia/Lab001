#pragma once

// SpinResult — это именно слепок результата вращения, а не сам процесс
// вращения.  поэтому это ValueObject

#include "PayoutEntry.hpp"
#include "SpinStatus.hpp"

namespace Values {

    class SpinResult {
        RouletteNumber number;
        SpinStatus status;  // Completed, Cancelled
        std::vector<PayoutEntry> payouts;

       public:
        SpinResult(RouletteNumber rn, SpinStatus ss,
                   std::vector<PayoutEntry> pe);
        const RouletteNumber& number() const;
        SpinStatus status() const;
        const std::vector<PayoutEntry>& payouts() const;
        bool operator==(const SpinResult&) const;
    };
}  // namespace Values