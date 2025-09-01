#include "board.h"
#include "fen.h"
#include "moves.h"
#include "piece.h"
#include <iostream>

inline bool in_range(int x, int a, int b) {
  if (a > b)
    std::swap(a, b);
  return x >= a && x <= b;
}

Board::Board() {
  Fen::load(*this, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
  // load_fen(*this, "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR");
}

void Board::set_piece(int square, int piece) {
  if (!in_range(square, 0, SQUARES - 1))
    return;
  squares[square] = piece;
}

void Board::undo_move() {
  if (undo_list.empty())
    return;

  UndoMove &move = undo_list.back();
  move_piece(move.to, move.from);
  set_piece(move.to, move.captured_piece);
  en_passant_square = move.en_passant_square;
  new_turn();

  undo_list.pop_back();
}

void Board::do_move(Move &move) {
  int captured_piece = piece_at(move.to);
  move_piece(move.from, move.to);
  undo_list.push_back({move.from, move.to, captured_piece, en_passant_square});

  new_turn();
}

int Board::piece_at(int index) {
  if (in_range(index, 0, 63))
    return squares[index];
  std::cerr << "Tried to access an out of bounds index" << std::endl;
  return 0;
}

void Board::move_piece(int from, int to) {
  squares[to] = squares[from];
  squares[from] = Piece::EMPTY;
}

bool Board::is_square_empty(int index) {
  return piece_at(index) == Piece::EMPTY;
}
