#include "board.h"
#include "perft.h"
#include <gtest/gtest.h>

TEST(SpeedTest, perft) {
  Board board{};

  ASSERT_EQ(perft(6, board), 119060324);
}
