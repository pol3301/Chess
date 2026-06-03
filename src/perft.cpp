#include "board.h"
#include "moves.h"
#include <cstdint>
#include <iostream>
// using std::uint64_t;

uint64_t perft(int depth, Board &board) {
  // Base case
  if (depth == 0)
    return 1ULL;

  MoveGenerator mg;
  mg.generate_legal_moves(board);

  // Bulk counting optimization (optional, but highly recommended for speed)
  if (depth == 1)
    return mg.legal_moves.size();

  std::uint64_t nodes = 0;
  int n_moves = mg.legal_moves.size();

  for (int i = 0; i < n_moves; i++) {
    board.do_move(mg.legal_moves[i]);
    nodes += perft(depth - 1, board);
    board.undo_move();
  }

  return nodes;
}

// The root function to handle the "divide" printing
void perft_divide(int depth, Board &board) {
  if (depth == 0)
    return;

  MoveGenerator mg;
  mg.generate_legal_moves(board);

  std::uint64_t total_nodes = 0;
  int n_moves = mg.legal_moves.size();

  for (int i = 0; i < n_moves; i++) {
    board.do_move(mg.legal_moves[i]);
    std::uint64_t child_nodes = perft(depth - 1, board);
    total_nodes += child_nodes;
    board.undo_move();

    // Print the breakdown for this root move
    std::cout << move_to_string(mg.legal_moves[i]) << ": " << child_nodes
              << std::endl;
  }

  // Print the final result
  std::cout << "\nTotal nodes: " << total_nodes << std::endl;
}

void do_perft(std::string half_turns) {

  Board board;
  // int count = perft(std::stoi(half_turns), board);
  perft_divide(std::stoi(half_turns), board);

  // std::cout << count << std::endl;
}
