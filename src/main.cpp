#include "game.h"
#include "perft.h"

int main(int argc, char *argv[]) {

  if (argc > 1) {
    do_perft(argv[1]);
  } else {
    Game game;
    game.run();
  }
}
