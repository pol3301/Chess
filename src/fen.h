#pragma once

class Board;

class Fen {
public:
  static constexpr char start_pos[] =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

  static void load(Board &board, const char *fen);

private:
  static void set_piece(int index, char token, Board &board);
  static bool is_fen_char(char token);
};
