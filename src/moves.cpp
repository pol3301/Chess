#include "moves.h"
#include "board.h"
#include "piece.h"
#include <algorithm>
#include <array>
#include <bit>
#include <vector>

int MoveGenerator::to_edge[Board::SQUARES][8] = {0};
bitboard MoveGenerator::knight_bitboards[Board::SQUARES];

// clang-format off
constexpr std::array<const char*, 64> tile_table = {
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};
// clang-format on

std::string move_to_string(Move move) {
  std::string string;

  string.append(tile_table[move.from]);
  string.append(tile_table[move.to]);

  return string;
}

void MoveGenerator::precompute_move_data() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      int i = x + y * 8;

      int north = 7 - y;
      int south = y;
      int east = 7 - x;
      int west = x;

      to_edge[i][NORTH] = north;
      to_edge[i][SOUTH] = south;
      to_edge[i][EAST] = east;
      to_edge[i][WEST] = west;

      to_edge[i][NORTH_EAST] = std::min(north, east);
      to_edge[i][NORTH_WEST] = std::min(north, west);
      to_edge[i][SOUTH_EAST] = std::min(south, east);
      to_edge[i][SOUTH_WEST] = std::min(south, west);
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
    bitboard bb = 0;

    int x = curr_square % 8;
    int y = curr_square / 8;

    for (int i = 0; i < 8; i++) {
      int index = curr_square + knight_directions[i];

      int x2 = index % 8;
      int y2 = index / 8;

      if (std::abs(x - x2) > 2 || std::abs(y - y2) > 2)
        continue;

      if (index < 0 || index >= Board::SQUARES)
        continue;

      bb |= 1ULL << index;
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
    promotion_rank = 7;
  } else {
    dir = -8;
    capture_dir = capture_dir_black;
    start_rank = 6;
    promotion_rank = 0;
  }

  int one_step = piece_index + dir;
  int two_step = piece_index + (dir * 2);

  if (board.is_square_empty(one_step)) {
    if (one_step / 8 != promotion_rank) {
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
    int edges[] = {EAST, WEST};

    if (to_edge[piece_index][edges[i]] < 1)
      continue;

    int color = Piece::color(board.piece_at(index));

    if (!board.is_square_empty(index) && color != moving_color) {
      if (index / 8 != promotion_rank) {
        moves.push_back({piece_index, index, FLAG_NONE});
      } else {
        moves.push_back({piece_index, index, FLAG_PROMOTE_QUEEN});
        moves.push_back({piece_index, index, FLAG_PROMOTE_ROOK});
        moves.push_back({piece_index, index, FLAG_PROMOTE_BISHOP});
        moves.push_back({piece_index, index, FLAG_PROMOTE_KNIGHT});
      }
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

std::vector<Move> MoveGenerator::generate_castles(Board &board,
                                                  std::vector<Move> &moves) {
  std::vector<Move> castling_moves;
  moves.reserve(4);

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

  if (can_castle_king && !(all_pieces & mask_king) && can_move_right) {
    castling_moves.push_back({4 + base_rank, 6 + base_rank, FLAG_KING_CASTLE});
  }

  if (can_castle_queen && !(all_pieces & mask_queen) && can_move_left) {
    castling_moves.push_back({4 + base_rank, 2 + base_rank, FLAG_QUEEN_CASTLE});
  }

  return castling_moves;
}

void MoveGenerator::generate_legal_moves(Board &board) {

  std::vector<Move> moves = generate_pseudo_legal_moves(board);

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

  if (board.get_castling_rights() != 0) {
    board.new_turn();
    std::vector<Move> enemy_moves = generate_pseudo_legal_moves(board);

    bool king_in_danger = false;
    for (Move enemy_move : enemy_moves) {
      if (Piece::type(board.piece_at(enemy_move.to)) == Piece::KING) {
        king_in_danger = true;
        break;
      }
    }
    board.new_turn();

    if (!king_in_danger) {
      std::vector<Move> castles = generate_castles(board, moves);

      if (castles.size() != 0) {
        for (Move castle : castles) {
          board.do_move(castle);

          std::vector<Move> enemy_moves = generate_pseudo_legal_moves(board);

          bool king_in_danger = false;
          for (Move enemy_move : enemy_moves) {
            if (Piece::type(board.piece_at(enemy_move.to)) == Piece::KING) {
              king_in_danger = true;
              break;
            }
          }

          if (!king_in_danger)
            moves.push_back(castle);

          board.undo_move();
        }
      }
    }
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
