#pragma once
#include <vector>
// SpinResult — это именно слепок результата вращения, а не сам процесс
// вращения.  поэтому это ValueObject

#include "PayoutEntry.hpp"
#include "RouletteNumber.hpp"
#include "SpinStatus.hpp"

namespace Values {

    class SpinResult {
       private:
        RouletteNumber number;
        SpinStatus status;  // Completed, Cancelled
        std::vector<PayoutEntry> payouts;

       public:
        SpinResult(RouletteNumber number, SpinStatus status,
                   std::vector<PayoutEntry> payouts)
            : number(number), status(status), payouts(std::move(payouts)) {}

        const RouletteNumber& get_number() const { return number; }
        SpinStatus get_status() const { return status; }
        const std::vector<PayoutEntry>& get_payouts() const { return payouts; }
    };

}  // namespace Values
