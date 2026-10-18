/* #include "Bet.hpp"
#include "Money.hpp"
#include "PayoutEntry.hpp"
#include "Player.hpp"
#include "PlayerId.hpp"
 */
namespace Ports {
    //    using Bets::Bet;
    // using Domain::Player;
    //    using Values::Money;
    //    using Values::PlayerId;
    struct Logger {
        virtual ~Logger() = default;
        virtual void log(const std::string& message) = 0;
        /*
                virtual void log_payout(PlayerId player_id, Money amount) = 0;
                virtual void log_balance(Player player) = 0;
                virtual void log_bet(PlayerId player_id, Bet& bet) = 0;
                virtual void log_exceptions(std::string str) = 0;
          */
    };

}  // namespace Ports