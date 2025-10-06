#include "board.h"
#include "game.h"
#include "moves.h"
#include <iostream>

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

std::uint64_t perft(int depth, Board &board) {

  static int initial_depth = 0;

  if (initial_depth == 0)
    initial_depth = depth;

  MoveGenerator mg;

  int n_moves, i;
  std::uint64_t nodes = 0;

  if (depth == 0)
    return 1ULL;

  mg.generate_legal_moves(board);

  n_moves = mg.legal_moves.size();

  for (i = 0; i < n_moves; i++) {
    board.do_move(mg.legal_moves[i]);

    std::int64_t child_nodes = perft(depth - 1, board);
    nodes += child_nodes;

    board.undo_move();

    if (depth == initial_depth) {
      std::cout << move_to_string(mg.legal_moves[i]) << ": " << child_nodes
                << std::endl;
    }
  }

  return nodes;
}

void do_perft(std::string half_turns) {

  Board board;
  int count = perft(std::stoi(half_turns), board);

  std::cout << count << std::endl;
}

int main(int argc, char *argv[]) {

  if (argc > 1) {
    do_perft(argv[1]);
  } else {
    Game game;
    game.run();
  }
}
