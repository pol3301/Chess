#include "game.h"
#include "perft.h"

struct Vec3d {
  int x, y, z;
};

int main(int argc, char *argv[]) {

  if (argc > 1) {
    do_perft(argv[1]);
  } else {
    Game game;
    game.run();

    Vec3d vector{1, 2, 3};
  }
}
