#pragma once

#include "board.h"
#include "moves.h"
#include "renderer.h"
#include <SDL.h>

class Game {
public:
  Game();
  ~Game();
  void run();

private:
  SDL_Window *window;
  Renderer renderer;
  MoveGenerator mh;
  Board board;

  HeldPiece held_piece = {};

  bool is_running = true;

  void update(SDL_Event *e);
};
