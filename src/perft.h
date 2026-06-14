#pragma once

#include "board.h"
#include <string>

void do_perft(std::string half_turns);
std::uint64_t perft(int depth, Board &board);
void perft_divide(int depth, Board &board);
