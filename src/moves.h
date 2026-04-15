#pragma once

#include "board.h"
#include <vector>

enum move_flags {
  FLAG_NONE,
  FLAG_DOUBLE_PAWN_PUSH,
  FLAG_EN_PASSANT,
  FLAG_QUEEN_CASTLE,
  FLAG_KING_CASTLE,
  FLAG_PROMOTE_QUEEN,
  FLAG_PROMOTE_ROOK,
  FLAG_PROMOTE_BISHOP,
  FLAG_PROMOTE_KNIGHT,
};

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

  std::vector<Move> legal_moves;

  static void precompute_move_data();

  enum {
    CASTLING_WK = 0b1000,
    CASTLING_WQ = 0b0100,
    CASTLING_BK = 0b0010,
    CASTLING_BQ = 0b0001
  };

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

  static constexpr int sliding_offsets[8] = {8, -8, 1, -1, 7, 9, -9, -7};
  static constexpr int knight_directions[8] = {6,  10,  17,  15,
                                               -6, -10, -17, -15};

  static constexpr bitboard castle_masks[4] = {
      0b01100000ULL,
      0b00001110ULL,
      0b01100000ULL << 56,
      0b00001110ULL << 56,
  };

  static bitboard knight_bitboards[64];

  static void calculate_knight_moves();

  std::vector<Move> generate_pseudo_legal_moves(Board &board);

  void generate_castles(Board &board, std::vector<Move> &moves);
  void generate_king_moves(Board &board, int piece_index,
                           std::vector<Move> &moves);
  void generate_knight_moves(Board &board, int piece_index,
                             std::vector<Move> &moves);
  void generate_sliding_moves(Board &board, int piece_index,
                              std::vector<Move> &moves);
  void generate_pawn_moves(Board &board, int piece_index,
                           std::vector<Move> &moves);

  static int to_edge[Board::SQUARES][8];
};
