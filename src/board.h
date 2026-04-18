#pragma once

#include "piece.h"
#include <cstdint>
#include <list>
#include <string>

using bitboard = std::uint64_t;

struct Move;

struct UndoMove {
  int from, to;
  int captured_piece;
  int en_passant_square;
  int castle_rights;
  int flags;
};

struct HeldPiece {
  int index, x, y;
  bool is_piece_held;
};

class Board {
public:
  static constexpr int SQUARES = 64;
  static constexpr int EN_PASSANT_NULL = 999;

  Board();
  Board(std::string fen);

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

  int en_passant_square;

  bitboard white_bitboard = 0;
  bitboard black_bitboard = 0;

private:
  inline void update_castling_rights(int from, int to);
  inline void handle_promotion(int flag, int from, int to);
  inline void handle_special_cases(const Move &move, int moved_color,
                                   UndoMove &undo);
  std::list<UndoMove> undo_list;
  int castling_rights = 0b1111;
  int squares[SQUARES] = {0};
  int turn = Piece::WHITE;
};
