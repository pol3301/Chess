#include "moves.h"
#include "board.h"
#include "piece.h"
#include <algorithm>

int MoveGenerator::to_edge[Board::SQUARES][8] = {0};

void MoveGenerator::precompute_move_data() {
  int i;
  int north, south, east, west;
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      i = x + y * 8;

      north = 7 - y;
      south = y;
      east = 7 - x;
      west = x;

      to_edge[i][NORTH_EAST] = std::min(north, east);
      to_edge[i][NORTH_WEST] = std::min(north, west);
      to_edge[i][SOUTH_EAST] = std::min(south, east);
      to_edge[i][SOUTH_WEST] = std::min(south, west);

      to_edge[i][NORTH] = north;
      to_edge[i][SOUTH] = south;
      to_edge[i][EAST] = east;
      to_edge[i][WEST] = west;
    }
  }
}

void MoveGenerator::generate_sliding_moves(Board &board, int piece_index) {
  int piece = board.piece_at(piece_index);
  int piece_type = Piece::type(piece);

  int start = piece_type == Piece::BISHOP ? 4 : 0;
  int end = piece_type == Piece::ROOK ? 4 : 8;

  for (int slide_dir = start; slide_dir < end; slide_dir++) {
    int index = piece_index;
    for (int i = 0; i < to_edge[piece_index][slide_dir]; i++) {
      index = piece_index + (sliding_offsets[slide_dir] * (i + 1));

      if (!board.is_square_empty(index)) {
        if (Piece::color(board.piece_at(index)) != Piece::color(piece))
          legal_moves.push_back({piece_index, index, FLAG_NONE});

        break;
      } else
        legal_moves.push_back({piece_index, index, FLAG_NONE});
    }
  }
}

void MoveGenerator::generate_king_moves(Board &board, int piece_index) {
  int piece = board.piece_at(piece_index);

  for (int slide_dir = 0; slide_dir < 8; slide_dir++) {
    if (to_edge[piece_index][slide_dir] < 1)
      continue;
    int index = piece_index;
    index = piece_index + sliding_offsets[slide_dir];

    if (!board.is_square_empty(index)) {
      if (Piece::color(board.piece_at(index)) != Piece::color(piece))
        legal_moves.push_back({piece_index, index, FLAG_NONE});

      continue;
    } else
      legal_moves.push_back({piece_index, index, FLAG_NONE});
  }
}

constexpr int knight_directions[] = {6, 10, 17, 15, -6, -10, -17, -15};

void MoveGenerator::generate_knight_moves(Board &board, int piece_index) {
  int piece_color = Piece::color(board.piece_at(piece_index));
  int x, y;
  x = piece_index % 8;
  y = piece_index / 8;

  for (int i = 0; i < 8; i++) {
    int index = piece_index + knight_directions[i];

    int x2, y2;
    x2 = index % 8;
    y2 = index / 8;

    if (std::abs(x - x2) > 2 || std::abs(y - y2) > 2)
      continue;

    if (index < 0 || index >= Board::BOARD_SIZE)
      continue;
    if (Piece::color(board.piece_at(index)) == piece_color)
      continue;
    legal_moves.push_back({piece_index, index, FLAG_NONE});
  }
}

void MoveGenerator::generate_pawn_moves(Board &board, int piece_index) {
  int moving_color = Piece::color(board.piece_at(piece_index));

  int dir, capture_dir[2], start_rank;

  if (moving_color == Piece::WHITE) {
    dir = 8;
    capture_dir[0] = 7;
    capture_dir[1] = 9;
    start_rank = 1;
  } else {
    dir = -8;
    capture_dir[0] = -7;
    capture_dir[1] = -9;
    start_rank = 6;
  }

  int one_step = piece_index + dir;
  int two_step = piece_index + (dir * 2);

  if (board.is_square_empty(one_step)) {
    legal_moves.push_back({piece_index, one_step, FLAG_NONE});

    if (piece_index / 8 == start_rank && board.is_square_empty(two_step))
      legal_moves.push_back({piece_index, two_step, DOUBLE_PAWN_PUSH});
  }

  for (int i = 0; i < 2; i++) {
    int index = piece_index + capture_dir[i];
    int color = Piece::color(board.piece_at(index));

    if (!board.is_square_empty(index) && color != moving_color) {
      legal_moves.push_back({piece_index, index, FLAG_NONE});
    }
  }
}

void MoveGenerator::generate_legal_moves(Board &board) {
  generate_pseudo_legal_moves(board);
}

void MoveGenerator::generate_pseudo_legal_moves(Board &board) {
  legal_moves.clear();
  for (int i = 0; i < Board::SQUARES; i++) {
    if (Piece::color(board.piece_at(i)) != board.get_turn())
      continue;
    switch (Piece::type(board.piece_at(i))) {
    case Piece::KING:
      generate_king_moves(board, i);
      break;
    case Piece::KNIGHT:
      generate_knight_moves(board, i);
      break;
    case Piece::PAWN:
      generate_pawn_moves(board, i);
      break;
    case Piece::QUEEN:
    case Piece::ROOK:
    case Piece::BISHOP:
      generate_sliding_moves(board, i);
      break;
    }
  }
}
