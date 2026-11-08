#include <gtest/gtest.h>

#include "PlayerId.hpp"
#include "SpinResult.hpp"
#include "SpinStatus.hpp"

TEST(SpinResultTest, CarriesNumberStatusAndPayouts) {
    Values::PayoutEntry e1(Values::PlayerId(1), Values::Money(1000'00));
    Values::PayoutEntry e2(Values::PlayerId(2), Values::Money(2000'00));

    Values::SpinResult result(Values::RouletteNumber(17),
                              Values::SpinStatus::Completed,
                              {e1, e2});

    EXPECT_EQ(result.get_number().value(), 17);
    EXPECT_EQ(result.get_status(), Values::SpinStatus::Completed);
    ASSERT_EQ(result.get_payouts().size(), 2u);
    EXPECT_EQ(result.get_payouts()[0].get_player().get_id(), 1);
    EXPECT_EQ(result.get_payouts()[0].get_amount(), Values::Money(1000'00));
    EXPECT_EQ(result.get_payouts()[1].get_player().get_id(), 2);
    EXPECT_EQ(result.get_payouts()[1].get_amount(), Values::Money(2000'00));
}

TEST(SpinResultTest, CancelledStatusWithRefundPayouts) {
    Values::PayoutEntry refund(Values::PlayerId(1), Values::Money(500'00));
    Values::SpinResult result(Values::RouletteNumber(0),
                              Values::SpinStatus::Cancelled,
                              {refund});
    EXPECT_EQ(result.get_status(), Values::SpinStatus::Cancelled);
    ASSERT_EQ(result.get_payouts().size(), 1u);
    EXPECT_EQ(result.get_payouts()[0].get_amount(), Values::Money(500'00));
}
