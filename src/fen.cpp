#include "fen.h"
#include "board.h"
#include "piece.h"

#include <array>
#include <string>

bool Fen::is_fen_char(char token) {
  static const std::string valid("KQRBNPkqrbnp12345678-/");
  return valid.find(token) != std::string::npos;
}

void Fen::set_piece(int index, char token, Board &board) {
  int color = std::isupper(token) ? Piece::WHITE : Piece::BLACK;
  int base = std::toupper(token);

  static const std::array<int, 26> piece_lookup = [] {
    std::array<int, 26> arr{};
    arr['K' - 'A'] = Piece::KING;
    arr['Q' - 'A'] = Piece::QUEEN;
    arr['R' - 'A'] = Piece::ROOK;
    arr['B' - 'A'] = Piece::BISHOP;
    arr['N' - 'A'] = Piece::KNIGHT;
    arr['P' - 'A'] = Piece::PAWN;
    return arr;
  }();

  if (base < 'A' || base > 'Z')
    return;

  int piece_type = piece_lookup[base - 'A'];

  if (piece_type == 0)
    return;

  board.set_piece(index, piece_type | color);
}

void Fen::load(Board &board, const char *fen) {
  int x = 0, y = 7;
  const char *token = fen;
  board.set_turn(Piece::WHITE);
  board.en_passant_square = 0;

  while (*token != '\0') {

    if (!is_fen_char(*token))
      return;

    if (std::isalpha(*token)) {
      set_piece(x + y * 8, *token, board);
      x++;
    } else if (std::isdigit(*token)) {
      x += *token - '0';
    } else if (*token == '/') {
      x = 0;
      y--;
    }

    token++;
  }
}
