#include "board.h"
#include "fen.h"
#include "moves.h"
#include "piece.h"

inline bool in_range(int x, int a, int b) {
  if (a > b)
    std::swap(a, b);
  return x >= a && x <= b;
}

Board::Board() {
  Fen::load(*this, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

Board::Board(std::string fen) { Fen::load(*this, fen.c_str()); }

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

  switch (move.flags) {
  case FLAG_WHITE_KING_CASTLE:
    move_piece(5, 7);
    break;
  case FLAG_WHITE_QUEEN_CASTLE:
    move_piece(3, 0);
    break;
  case FLAG_BLACK_KING_CASTLE:
    move_piece(61, 63);
    break;
  case FLAG_BLACK_QUEEN_CASTLE:
    move_piece(58, 56);
    break;
  case FLAG_EN_PASSANT:
    if (in_range(move.to, 24, 31))
      set_piece(move.to + 8, move.captured_piece);
    else
      set_piece(move.to - 8, move.captured_piece);
    set_piece(move.to, Piece::EMPTY);
    break;
  }

  undo_list.pop_back();
}

void Board::do_move(Move &move) {
  int captured_piece = piece_at(move.to);
  UndoMove undo = {move.from,         move.to,         captured_piece,
                   en_passant_square, castling_rights, move.flags};

  en_passant_square = 0;
  move_piece(move.from, move.to);

  switch (move.flags) {
  case FLAG_WHITE_KING_CASTLE:
    move_piece(7, 5);
    break;
  case FLAG_WHITE_QUEEN_CASTLE:
    move_piece(0, 3);
    break;
  case FLAG_BLACK_KING_CASTLE:
    move_piece(63, 61);
    break;
  case FLAG_BLACK_QUEEN_CASTLE:
    move_piece(56, 59);
    break;
  case FLAG_DOUBLE_PAWN_PUSH:
    en_passant_square =
        move.to + (Piece::color(piece_at(move.from)) == Piece::WHITE ? -8 : 8);
    break;
  case FLAG_EN_PASSANT:
    if (in_range(move.to, 24, 31)) {
      undo.captured_piece = piece_at(move.to + 8);
      set_piece(move.to + 8, Piece::EMPTY);
    } else {
      undo.captured_piece = piece_at(move.to - 8);
      set_piece(move.to - 8, Piece::EMPTY);
    }
    break;
  }

  int squares[2] = {move.from, move.to};
  for (int i = 0; i < 2; i++) {
    switch (squares[i]) {
    case 4:
      castling_rights &= ~MoveGenerator::CASTLING_WK;
      castling_rights &= ~MoveGenerator::CASTLING_WQ;
      break;
    case 60:
      castling_rights &= ~MoveGenerator::CASTLING_BK;
      castling_rights &= ~MoveGenerator::CASTLING_BQ;
      break;
    case 0:
      castling_rights &= ~MoveGenerator::CASTLING_WQ;
      break;
    case 7:
      castling_rights &= ~MoveGenerator::CASTLING_WK;
      break;
    case 56:
      castling_rights &= ~MoveGenerator::CASTLING_BQ;
      break;
    case 63:
      castling_rights &= ~MoveGenerator::CASTLING_BK;
      break;
    }
  }

  new_turn();

  undo_list.push_back(undo);
}

void Board::move_piece(int from, int to) {
  int moved_color = Piece::color(piece_at(from));

  if (!is_square_empty(to)) {
    bitboard &taken_bitboard =
        (moved_color == Piece::WHITE ? black_bitboard : white_bitboard);

    taken_bitboard &= ~(1 << to);
  }

  set_piece(to, piece_at(from));
  set_piece(from, Piece::EMPTY);
}
