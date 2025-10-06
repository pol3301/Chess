#include "game.h"
#include "moves.h"
#include <SDL_image.h>
#include <algorithm>
#include <cstdint>
#include <iostream>

constexpr int FRAME_RATE = 60;
constexpr int FRAME_DELAY = 1000 / FRAME_RATE;

Game::Game() : window(nullptr), renderer(board) {

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    exit(-1);
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    std::cerr << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(-1);
  }

  window =
      SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << SDL_GetError() << std::endl;
    SDL_Quit();
    IMG_Quit();
    exit(-1);
  }

  renderer.init(window);
}

Game::~Game() {
  if (window)
    SDL_DestroyWindow(window);

  SDL_Quit();
  IMG_Quit();
}

void Game::update(SDL_Event *e) {
  int x, y;
  Move move;
  SDL_Keycode key_pressed;
  Uint16 key_mod;

  std::vector<Move>::iterator moveIndex;

  while (SDL_PollEvent(e)) {
    switch (e->type) {
    case SDL_QUIT:
      is_running = false;
    case SDL_MOUSEBUTTONDOWN:
      SDL_GetMouseState(&held_piece.x, &held_piece.y);

      x = held_piece.x / 100;
      y = held_piece.y / 100;

      y = 7 - y;

      held_piece.index = y * 8 + x;
      held_piece.is_piece_held = true;
      break;

    case SDL_MOUSEBUTTONUP:
      SDL_GetMouseState(&held_piece.x, &held_piece.y);
      held_piece.is_piece_held = false;

      x = held_piece.x / 100;
      y = held_piece.y / 100;

      y = 7 - y;

      move = {held_piece.index, y * 8 + x, 0};

      moveIndex = std::find(mg.legal_moves.begin(), mg.legal_moves.end(), move);
      if (moveIndex != mg.legal_moves.end()) {
        board.do_move(*moveIndex);
        mg.generate_legal_moves(board);
      }

      break;

    case SDL_MOUSEMOTION:
      SDL_GetMouseState(&held_piece.x, &held_piece.y);
      break;

    case SDL_KEYDOWN:
      key_pressed = e->key.keysym.sym;
      key_mod = e->key.keysym.mod;
      if (key_pressed == SDLK_ESCAPE)
        is_running = false;
      if (key_pressed == SDLK_n) {
        board.new_turn();
        mg.generate_legal_moves(board);
      }
      if ((key_mod & KMOD_CTRL) && key_pressed == SDLK_z) {
        board.undo_move();
        mg.generate_legal_moves(board);
      }

    default:
      break;
    }
  }
}

void Game::run() {
  SDL_Event e;

  mg.generate_legal_moves(board);
  while (is_running) {
    uint32_t frame_start = SDL_GetTicks();

    update(&e);
    renderer.render(board, &held_piece, mg.legal_moves);

    uint32_t frame_time = SDL_GetTicks() - frame_start;

    if (FRAME_DELAY > frame_time)
      SDL_Delay(FRAME_DELAY - frame_time);
  }
}
