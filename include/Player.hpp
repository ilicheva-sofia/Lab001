#pragma once
#include "Money.hpp"
#include "PlayerId.hpp"

namespace Domain {
    class Player {
       private:
        Values::PlayerId id;
        Values::Money balance;

       public:
        Player(Values::PlayerId id, Values::Money init_balance);

        Values::PlayerId get_id() const { return id; }
        Values::Money get_balance() const { return balance; }

        void receive_payout(Values::Money amount);  //  balance ++
        void spend_money(Values::Money amount);     //  balance --
    };
}  // namespace Domain
