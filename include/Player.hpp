#include "Money.hpp"
#include "PlayerId.hpp"

namespace entities {
    using namespace Values;

    class Player {
        PlayerId id;
        Money balance;

       public:
        Player(PlayerId id, Money init_balance)
            : id(id), balance(init_balance) {}

        PlayerId get_id() const { return id; }  // do we need it?
        const Money& balance() const { return balance; }
        void receive_payout(Money amount);  //  balance ++
        void place_bet(Money amount);       //  balance --
    };

}  // namespace entities
