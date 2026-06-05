#include "fen.h"
#include "board.h"
#include "moves.h"
#include "piece.h"

#include <array>
#include <iostream>

constexpr std::array<const char *, 64> tile_table = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2",
    "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3",
    "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a5",
    "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6",
    "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7", "e7", "f7", "g7",
    "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};

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

std::vector<std::string_view> split_strings(std::string_view fen) {
  std::vector<std::string_view> strings;
  size_t start = 0;
  size_t end = fen.find(' ');

  while (end != std::string_view::npos) {
    if (end != start) {
      strings.push_back(fen.substr(start, end - start));
    }
    start = end + 1;
    end = fen.find(' ', start);
  }

  if (start < fen.size()) {
    strings.push_back(fen.substr(start));
  }

  return strings;
}

int Fen::load(Board &board, std::string_view fen) {
  auto failed = [&]() {
    load(board, START_POS);
    std::cout << "Failed" << std::endl;
    return -1;
  };

  board.clear_board();

  int x = 0, y = 7;

  board.set_turn(Piece::WHITE);
  board.en_passant_square = Board::EN_PASSANT_NULL;

  std::vector<std::string_view> parts = split_strings(fen);

  if (parts.size() < 4)
    return failed();

  for (char token : parts[0]) {
    if (!is_fen_char(token)) {
      return failed();
    }

    if (std::isalpha(token)) {
      set_piece(x + y * 8, token, board);
      x++;
    } else if (std::isdigit(token)) {
      x += token - '0';
    } else if (token == '/') {
      if (x != 8) {
        return failed();
        break;
      }
      x = 0;
      y--;
    }
  }

  if (parts[1].size() > 1)
    return failed();
  else
    board.set_turn(parts[1][0] == 'w' ? Piece::WHITE : Piece::BLACK);

  if (parts[2].size() > 4)
    return failed();
  else {
    int rights = 0;
    for (char right : parts[2]) {
      switch (right) {
      case 'K':
        rights |= MoveGenerator::CASTLING_WK;
        break;
      case 'Q':
        rights |= MoveGenerator::CASTLING_WQ;
        break;
      case 'k':
        rights |= MoveGenerator::CASTLING_BK;
        break;
      case 'q':
        rights |= MoveGenerator::CASTLING_BQ;
        break;
      case '-':
        break;
      default:
        return failed();
        break;
      }
    }
    board.set_castling_rights(rights);
  }

  if (parts[3].size() > 2)
    return failed();
  else {
    if (parts[3] == "-")
      board.en_passant_square = Board::EN_PASSANT_NULL;
    else
      board.en_passant_square = parts[3][0] - 'a' + (parts[3][1] - '1') * 8;
    if ((board.en_passant_square > 64 || board.en_passant_square < 0) &&
        parts[3] != "-")
      return failed();
  }

  if (!(x == 8 && y == 0))
    return failed();

  return 0;
}

std::string Fen::generate_fen(Board &board) {
  std::string fen;

  int empty_count = 0;

  for (int y = 7; y >= 0; --y) {
    for (int x = 0; x < 8; ++x) {
      int index = y * 8 + x;

      int curr_piece = board.piece_at(index);

      if (curr_piece == Piece::EMPTY)
        empty_count++;

      else {
        if (empty_count > 0) {
          fen.append(std::to_string(empty_count));
          empty_count = 0;
        }

        char c = [curr_piece]() {
          switch (Piece::type(curr_piece)) {
          case Piece::KING:
            return 'k';

          case Piece::QUEEN:
            return 'q';
          case Piece::ROOK:
            return 'r';
          case Piece::BISHOP:
            return 'b';
          case Piece::KNIGHT:
            return 'n';
          case Piece::PAWN:
            return 'p';
          default:
            return 'x';
          }
        }();

        if (Piece::color(curr_piece) == Piece::WHITE)
          c = std::toupper(c);
        fen.push_back(c);
      }
    }

    if (empty_count > 0) {
      fen.append(std::to_string(empty_count));
      empty_count = 0;
    }

    if (y != 0)
      fen.append("/");
  }

  fen.push_back(' ');
  fen.push_back(board.get_turn() == Piece::WHITE ? 'w' : 'b');
  fen.push_back(' ');

  int rights = board.get_castling_rights();

  if (!rights)
    fen += '-';

  else {
    if (MoveGenerator::CASTLING_WK & rights)
      fen.push_back('K');
    if (MoveGenerator::CASTLING_WQ & rights)
      fen.push_back('Q');
    if (MoveGenerator::CASTLING_BK & rights)
      fen.push_back('k');
    if (MoveGenerator::CASTLING_BQ & rights)
      fen.push_back('q');
  }

  fen.push_back(' ');

  if (board.en_passant_square != Board::EN_PASSANT_NULL) {
    fen.append(tile_table[board.en_passant_square]);
  } else
    fen += '-';

  return fen;
}
