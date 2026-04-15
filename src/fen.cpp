#include "fen.h"
#include "board.h"
#include "piece.h"

#include <array>

bool Fen::is_fen_char(char token) {
  auto u_token = static_cast<unsigned char>(token);

  if (u_token > 127)
    return false;

  static constexpr std::array<bool, 128> valid_lookup = [] {
    std::array<bool, 128> legal_list{};

    for (char c : "KQRBNPkqrbnp12345678-/ ") {
      if (c != '\0')
        legal_list[c] = true;
    }

    return legal_list;
  }();

  return valid_lookup[u_token];
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

void Fen::load(Board &board, std::string_view fen) {
  int x = 0, y = 7;

  board.set_turn(Piece::WHITE);
  board.en_passant_square = board.EN_PASSANT_NULL;

  for (char token : fen) {

    if (!is_fen_char(token))
      return;

    if (std::isalpha(token)) {
      set_piece(x + y * 8, token, board);
      x++;
    } else if (std::isdigit(token)) {
      x += token - '0';
    } else if (token == '/') {
      x = 0;
      y--;
    }

    token++;
  }
}
