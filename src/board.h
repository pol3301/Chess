#pragma once

struct Move;

#include "piece.h"
#include <list>

struct UndoMove {
  int from, to;
  int captured_piece;
  int en_passant_square;
};

struct HeldPiece {
  int index, x, y;
  bool is_piece_held;
};

class Board {
public:
  static constexpr int SQUARES = 64;

  Board();

  void set_piece(int square, int type, int color);
  void set_piece(int square, int piece);

  void move_piece(int from, int to);

  void do_move(Move &move);
  void undo_move();

  int piece_at(int index);

  bool is_square_empty(int index);

  void new_turn() { turn = turn == Piece::WHITE ? Piece::BLACK : Piece::WHITE; }
  void set_turn(int new_turn) { turn = new_turn; }
  int get_turn() { return turn; }

  int en_passant_square;

private:
  std::list<UndoMove> undo_list;
  int squares[SQUARES] = {0};
  int turn;
};
