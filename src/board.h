#pragma once

#include "piece.h"
#include <cstdint>
#include <string>
#include <vector>

using bitboard = std::uint64_t;

struct Move;

// clang-format off
enum Square : int{
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
};
// clang-format on

struct UndoMove {
  int from, to;
  int captured_piece;
  int en_passant_square;
  int castle_rights;
  int flags;
};

struct HeldPiece {
  int index, x, y, piece;
  bool is_piece_held;
};

class Board {
public:
  static constexpr int SQUARES = 64;
  static constexpr int EN_PASSANT_NULL = 999;

  Board();
  Board(std::string fen);

  void clear_board();

  void set_piece(int square, int piece);

  void move_piece(int from, int to);

  void do_move(Move const &move);
  void undo_move();

  int piece_at(int index) { return squares[index]; }
  bool is_square_empty(int index) {
    return !((white_bitboard | black_bitboard) & 1ULL << index);
  }

  void new_turn() {
    set_turn(get_turn() == Piece::WHITE ? Piece::BLACK : Piece::WHITE);
  }
  void set_turn(int new_turn) { turn = new_turn; }
  int get_turn() { return turn; }

  int get_castling_rights() { return castling_rights; }
  inline void set_castling_rights(int new_castling_rights) {
    castling_rights = new_castling_rights;
  }

  int en_passant_square;

  bitboard white_bitboard = 0;
  bitboard black_bitboard = 0;

private:
  inline void update_castling_rights(int from, int to);
  inline void handle_promotion(int flag, int from, int to);
  inline void handle_special_cases(const Move &move, int moved_color,
                                   UndoMove &undo);
  std::vector<UndoMove> undo_list{};
  int castling_rights = 0b1111;
  int squares[SQUARES] = {0};
  int turn = Piece::WHITE;
};
