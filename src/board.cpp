#include "board.h"
#include "fen.h"
#include "moves.h"
#include "piece.h"
#include <iostream>

inline bool in_range(int x, int min, int max) {
  if (min > max)
    std::swap(min, max);
  return x >= min && x <= max;
}

Board::Board() {
  // Fen::load(*this, Fen::START_POS);
  Fen::load(
      *this,
      "r3k2r/p1p1qpb1/bn1ppnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/1R2K2R b Kkq -");
  std::cout << Fen::generate_fen(*this) << std::endl;
}

Board::Board(std::string fen) { Fen::load(*this, fen.c_str()); }

void Board::clear_board() {
  for (int i = 0; i < 64; ++i) {
    set_piece(i, Piece::EMPTY);
  }
}

void Board::set_piece(int square, int piece) {
  if (!in_range(square, 0, SQUARES - 1))
    return;

  int color = Piece::color(piece);

  white_bitboard &= ~(1ULL << square);
  black_bitboard &= ~(1ULL << square);

  if (color == Piece::WHITE)
    white_bitboard |= 1ULL << square;
  else if (color == Piece::BLACK)
    black_bitboard |= 1ULL << square;

  squares[square] = piece;
}

void Board::undo_move() {
  if (undo_list.empty())
    return;

  UndoMove &move = undo_list.back();

  move_piece(move.to, move.from);
  set_piece(move.to, move.captured_piece);
  en_passant_square = move.en_passant_square;
  castling_rights = move.castle_rights;

  new_turn();

  int moved_color = Piece::color(piece_at(move.from));

  switch (move.flags) {
  case FLAG_KING_CASTLE:
    if (moved_color == Piece::WHITE)
      move_piece(5, 7);
    else
      move_piece(61, 63);
    break;

  case FLAG_QUEEN_CASTLE:
    if (moved_color == Piece::WHITE)
      move_piece(3, 0);
    else
      move_piece(59, 56);
    break;

  case FLAG_EN_PASSANT:
    if (moved_color == Piece::WHITE)
      set_piece(move.to - 8, move.captured_piece);
    else
      set_piece(move.to + 8, move.captured_piece);
    set_piece(move.to, Piece::EMPTY);
    break;
  }

  if (in_range(move.flags, FLAG_PROMOTE_QUEEN, FLAG_PROMOTE_KNIGHT))
    set_piece(move.from, Piece::PAWN || moved_color);

  undo_list.pop_back();
}

inline void Board::handle_promotion(int flag, int from, int to) {
  switch (flag) {
  case FLAG_PROMOTE_QUEEN:
    set_piece(to, Piece::color(piece_at(to)) | Piece::QUEEN);
    break;
  case FLAG_PROMOTE_ROOK:
    set_piece(to, Piece::color(piece_at(to)) | Piece::ROOK);
    break;
  case FLAG_PROMOTE_BISHOP:
    set_piece(to, Piece::color(piece_at(to)) | Piece::BISHOP);
    break;
  case FLAG_PROMOTE_KNIGHT:
    set_piece(to, Piece::color(piece_at(to)) | Piece::KNIGHT);
    break;
  }
}

// TODO: Update to a lookup table
inline void Board::update_castling_rights(int from, int to) {
  for (int square : {from, to}) {
    switch (square) {
    case E1:
      castling_rights &= ~MoveGenerator::CASTLING_WK;
      castling_rights &= ~MoveGenerator::CASTLING_WQ;
      break;
    case E8:
      castling_rights &= ~MoveGenerator::CASTLING_BK;
      castling_rights &= ~MoveGenerator::CASTLING_BQ;
      break;
    case A1:
      castling_rights &= ~MoveGenerator::CASTLING_WQ;
      break;
    case H1:
      castling_rights &= ~MoveGenerator::CASTLING_WK;
      break;
    case A8:
      castling_rights &= ~MoveGenerator::CASTLING_BQ;
      break;
    case H8:
      castling_rights &= ~MoveGenerator::CASTLING_BK;
      break;
    }
  }
}

inline void Board::handle_special_cases(const Move &move, int moved_color,
                                        UndoMove &undo) {
  switch (move.flags) {
  case FLAG_KING_CASTLE:
    if (moved_color == Piece::WHITE)
      move_piece(7, 5);
    else
      move_piece(63, 61);
    break;

  case FLAG_QUEEN_CASTLE:
    if (moved_color == Piece::WHITE)
      move_piece(0, 3);
    else
      move_piece(56, 59);
    break;

  case FLAG_DOUBLE_PAWN_PUSH:
    en_passant_square = move.to + (moved_color == Piece::WHITE ? -8 : 8);
    break;

  case FLAG_EN_PASSANT:
    if (moved_color == Piece::WHITE) {
      undo.captured_piece = piece_at(move.to - 8);
      set_piece(move.to - 8, Piece::EMPTY);
    } else {
      undo.captured_piece = piece_at(move.to + 8);
      set_piece(move.to + 8, Piece::EMPTY);
    }
    break;
  }
}

void Board::do_move(Move const &move) {
  int captured_piece = piece_at(move.to);
  UndoMove undo = {move.from,         move.to,         captured_piece,
                   en_passant_square, castling_rights, move.flags};

  en_passant_square = EN_PASSANT_NULL;

  handle_special_cases(move, Piece::color(piece_at(move.from)), undo);

  move_piece(move.from, move.to);

  handle_promotion(move.flags, move.from, move.to);

  update_castling_rights(move.from, move.to);

  new_turn();

  undo_list.push_back(undo);
}

void Board::move_piece(int from, int to) {
  set_piece(to, piece_at(from));
  set_piece(from, Piece::EMPTY);
}
