#include "moves.h"
#include "board.h"
#include "piece.h"
#include <algorithm>
#include <bit>
// #include <iostream>

int MoveGenerator::to_edge[Board::SQUARES][8] = {0};
bitboard MoveGenerator::knight_bitboards[64];

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

  calculate_knight_moves();
}

void MoveGenerator::generate_sliding_moves(Board &board, int piece_index,
                                           std::vector<Move> &moves) {
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
          moves.push_back({piece_index, index, FLAG_NONE});

        break;
      } else
        moves.push_back({piece_index, index, FLAG_NONE});
    }
  }
}

void MoveGenerator::generate_king_moves(Board &board, int piece_index,
                                        std::vector<Move> &moves) {
  int piece = board.piece_at(piece_index);

  for (int slide_dir = 0; slide_dir < 8; slide_dir++) {
    if (to_edge[piece_index][slide_dir] < 1)
      continue;
    int index = piece_index;
    index = piece_index + sliding_offsets[slide_dir];

    if (!board.is_square_empty(index)) {
      if (Piece::color(board.piece_at(index)) != Piece::color(piece))
        moves.push_back({piece_index, index, FLAG_NONE});

      continue;
    } else
      moves.push_back({piece_index, index, FLAG_NONE});
  }
}

void MoveGenerator::calculate_knight_moves() {
  for (int curr_square = 0; curr_square < Board::SQUARES; ++curr_square) {
    // int piece_color = Piece::color(board.piece_at(curr_square));
    bitboard bb = 0;

    int x, y;
    x = curr_square % 8;
    y = curr_square / 8;

    for (int i = 0; i < 8; i++) {
      int index = curr_square + knight_directions[i];

      int x2, y2;
      x2 = index % 8;
      y2 = index / 8;

      if (std::abs(x - x2) > 2 || std::abs(y - y2) > 2)
        continue;

      if (index < 0 || index >= Board::SQUARES)
        continue;

      bb |= 1ULL << index;

      // if (Piece::color(board.piece_at(index)) == piece_color)
      // continue;
      // legal_moves.push_back({piece_index, index, FLAG_NONE});
    }

    knight_bitboards[curr_square] = bb;
  }
}

void MoveGenerator::generate_pawn_moves(Board &board, int piece_index,
                                        std::vector<Move> &moves) {
  int moving_color = Piece::color(board.piece_at(piece_index));

  int dir, start_rank, promotion_rank;
  int *capture_dir;

  static int capture_dir_white[2] = {9, 7};
  static int capture_dir_black[2] = {-7, -9};

  if (moving_color == Piece::WHITE) {
    dir = 8;
    capture_dir = capture_dir_white;
    start_rank = 1;
    promotion_rank = 8;
  } else {
    dir = -8;
    capture_dir = capture_dir_black;
    start_rank = 6;
    promotion_rank = 0;
  }

  int one_step = piece_index + dir;
  int two_step = piece_index + (dir * 2);

  if (board.is_square_empty(one_step)) {
    if (piece_index / 8 != promotion_rank) {
      moves.push_back({piece_index, one_step, FLAG_NONE});
    } else {
      moves.push_back({piece_index, one_step, FLAG_PROMOTE_QUEEN});
      moves.push_back({piece_index, one_step, FLAG_PROMOTE_ROOK});
      moves.push_back({piece_index, one_step, FLAG_PROMOTE_BISHOP});
      moves.push_back({piece_index, one_step, FLAG_PROMOTE_KNIGHT});
    }

    if (piece_index / 8 == start_rank && board.is_square_empty(two_step))
      moves.push_back({piece_index, two_step, FLAG_DOUBLE_PAWN_PUSH});
  }

  for (int i = 0; i < 2; i++) {
    int dir = capture_dir[i];

    int index = piece_index + dir;
    int color = Piece::color(board.piece_at(index));

    int edges[] = {EAST, WEST};

    if (to_edge[piece_index][edges[i]] < 1)
      continue;

    if (!board.is_square_empty(index) && color != moving_color) {
      moves.push_back({piece_index, index, FLAG_NONE});
    } else if (index == board.en_passant_square)
      moves.push_back({piece_index, index, FLAG_EN_PASSANT});
  }
}

void MoveGenerator::generate_knight_moves(Board &board, int piece_index,
                                          std::vector<Move> &moves) {
  bitboard moves_bb = knight_bitboards[piece_index];
  int my_color = Piece::color(board.piece_at(piece_index));

  while (moves_bb) {
    int to = std::countr_zero(moves_bb);
    moves_bb &= moves_bb - 1;

    if (my_color == Piece::color(board.piece_at(to)))
      continue;

    moves.push_back({piece_index, to, 0});
  }
}

void MoveGenerator::generate_castles(Board &board, std::vector<Move> &moves) {
  int rights = board.get_castling_rights();
  bool is_white_turn = board.get_turn() == Piece::WHITE;

  bool can_castle_king = rights & (is_white_turn ? CASTLING_WK : CASTLING_BK);
  bool can_castle_queen = rights & (is_white_turn ? CASTLING_WQ : CASTLING_BQ);

  bitboard mask_king = is_white_turn ? castle_masks[0] : castle_masks[2];
  bitboard mask_queen = is_white_turn ? castle_masks[1] : castle_masks[3];

  bitboard all_pieces = board.white_bitboard | board.black_bitboard;

  int base_rank = is_white_turn ? 0 : 7 * 8;

  Move right = {4 + base_rank, 5 + base_rank, FLAG_NONE};
  Move left = {4 + base_rank, 3 + base_rank, FLAG_NONE};

  bool can_move_left =
      std::find(moves.begin(), moves.end(), left) != moves.end();
  bool can_move_right =
      std::find(moves.begin(), moves.end(), right) != moves.end();

  int flag;

  if (can_castle_king && !(all_pieces & mask_king) && can_move_right) {
    moves.push_back({4 + base_rank, 6 + base_rank, FLAG_KING_CASTLE});
  }

  if (can_castle_queen && !(all_pieces & mask_queen) && can_move_left) {
    moves.push_back({4 + base_rank, 2 + base_rank, FLAG_QUEEN_CASTLE});
  }
}

void MoveGenerator::generate_legal_moves(Board &board) {

  std::vector<Move> moves = generate_pseudo_legal_moves(board);

  generate_castles(board, moves);

  for (int i = moves.size() - 1; i >= 0; --i) {
    board.do_move(moves[i]);

    std::vector<Move> enemy_moves = generate_pseudo_legal_moves(board);

    bool king_in_danger = false;
    for (Move enemy_move : enemy_moves) {
      if (Piece::type(board.piece_at(enemy_move.to)) == Piece::KING) {
        king_in_danger = true;
        break;
      }
    }

    if (king_in_danger) {
      moves.erase(moves.begin() + i);
    }

    board.undo_move();
  }

  legal_moves = moves;
}

std::vector<Move> MoveGenerator::generate_pseudo_legal_moves(Board &board) {
  bitboard pieces = board.get_turn() == Piece::WHITE ? board.white_bitboard
                                                     : board.black_bitboard;

  std::vector<Move> moves;
  moves.reserve(256);

  while (pieces) {
    int i = std::countr_zero(pieces);
    pieces &= pieces - 1;

    switch (Piece::type(board.piece_at(i))) {
    case Piece::KING:
      generate_king_moves(board, i, moves);
      break;
    case Piece::KNIGHT:
      generate_knight_moves(board, i, moves);
      break;
    case Piece::PAWN:
      generate_pawn_moves(board, i, moves);
      break;
    default:
      generate_sliding_moves(board, i, moves);
      break;
    }
  }

  return moves;
}
