#pragma once

#include "board.h"
#include "fen.h"
#include "inputhandler.h"
#include "moves.h"
#include "renderer.h"
#include <SDL.h>

struct HeldPiece {
  int index, x, y, piece;
  bool is_piece_held;
};

enum class GameState { PLAYING, PROMOTING, MENU };

class Game {
public:
  Game();
  ~Game();
  void run();

  void quit() { is_running = false; }
  void pick_up_piece(int mouse_x, int mouse_y);
  void drop_piece(int mouse_x, int mouse_y);
  void move_held_piece(int mouse_x, int mouse_y);
  void undo_move() { board.undo_move(); }
  void copy_fen_to_clipboard();
  int paste_fen(std::string fen) { return Fen::load(board, fen); }

  GameState get_state() { return game_state; }

private:
  SDL_Window *window;
  Board board;
  MoveGenerator mg;
  Renderer renderer;
  InputHandler ihandler;

  GameState game_state = GameState::PLAYING;

  HeldPiece held_piece{};

  bool is_running = true;

  int init_libraries();
  void promotion_query(int index) { game_state = GameState::PROMOTING; }

  void update();
};
