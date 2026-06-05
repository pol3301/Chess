#pragma once
#include <SDL.h>

class Game;

class InputHandler {
public:
  void handle_input(Game &g);

private:
  void handle_playing_input(Game &g, SDL_Event &e, int mouse_x, int mouse_y);
  void handle_promoting_input(Game &g, SDL_Event &e, int mouse_x, int mouse_y);
};
