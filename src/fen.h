#pragma once

#include <string_view>

class Board;

class Fen {
public:
  static constexpr char START_POS[] =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

  static void load(Board &board, std::string_view fen);
  static std::string generate_fen(Board &board);

private:
  static void set_piece(int index, char token, Board &board);
  static bool is_fen_char(char token);
};
