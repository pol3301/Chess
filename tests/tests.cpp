#include "board.h"
#include "moves.h"
#include "perft.h"
#include <gtest/gtest.h>

TEST(MoveGeneration, kiwipete) {
  Board board{
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"};

  EXPECT_EQ(perft(1, board), 48);
  EXPECT_EQ(perft(2, board), 2039);
  EXPECT_EQ(perft(3, board), 97862);
  EXPECT_EQ(perft(4, board), 4085603);
}

TEST(MoveGeneration, position3) {

  Board board{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"};

  EXPECT_EQ(perft(1, board), 14);
  EXPECT_EQ(perft(2, board), 191);
  EXPECT_EQ(perft(3, board), 2812);
  EXPECT_EQ(perft(4, board), 43238);
  EXPECT_EQ(perft(5, board), 674624);
  EXPECT_EQ(perft(6, board), 11030083);
  EXPECT_EQ(perft(7, board), 178633661);
}

TEST(MoveGeneration, position4) {
  Board board{
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"};

  EXPECT_EQ(perft(1, board), 6);
  EXPECT_EQ(perft(2, board), 264);
  EXPECT_EQ(perft(3, board), 9467);
  EXPECT_EQ(perft(4, board), 422333);
  EXPECT_EQ(perft(5, board), 15833292);
}
