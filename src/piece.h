#pragma once

class Piece {
public:
  enum { EMPTY, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };
  enum { WHITE = 0b01000, BLACK = 0b10000 };

  static constexpr int type_mask = 0b0111;
  static constexpr int color_mask = 0b11000;

  static inline int type(int piece) { return piece & type_mask; }
  static inline int color(int piece) { return piece & color_mask; }
};
