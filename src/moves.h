#pragma once

#include "board.h"
#include <vector>

using bitboard = std::uint64_t;

enum move_flags { FLAG_NONE, DOUBLE_PAWN_PUSH };

struct Move {
  int from, to;
  int flags;

  bool operator==(const Move &other) const {
    return from == other.from && to == other.to;
  }
};

class Board;

class MoveGenerator {
public:
  MoveGenerator() { precompute_move_data(); }
  void generate_legal_moves(Board &board);
  void generate_pseudo_legal_moves(Board &board);

  std::vector<Move> legal_moves;

  static void precompute_move_data();

private:
  enum directions {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST
  };

  static constexpr int sliding_offsets[] = {8, -8, 1, -1, 7, 9, -9, -7};

  void generate_king_moves(Board &board, int piece_index);
  void generate_knight_moves(Board &board, int piece_index);
  void generate_sliding_moves(Board &board, int piece_index);
  void generate_pawn_moves(Board &board, int piece_index);

  static int to_edge[Board::SQUARES][8];
};
