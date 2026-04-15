#pragma once

#include <string_view>

class Board;

class Fen {
public:
  static constexpr char start_pos[] =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

  static void load(Board &board, std::string_view fen);

private:
  static void set_piece(int index, char token, Board &board);
  static bool is_fen_char(char token);
};
