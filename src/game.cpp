#include "game.h"
#include "board.h"
#include "fen.h"
#include "moves.h"
#include <SDL_clipboard.h>
#include <SDL_image.h>
#include <SDL_keycode.h>
#include <SDL_stdinc.h>
#include <cstddef>
#include <cstdint>
#include <iostream>

constexpr int FRAME_RATE = 60;
constexpr int FRAME_DELAY = 1000 / FRAME_RATE;

Game::Game() : window(nullptr) {
  if (init_libraries() != 0)
    is_running = false;

  if (renderer.init() != 0)
    is_running = false;
}

Game::~Game() {
  if (window)
    SDL_DestroyWindow(window);

  SDL_Quit();
  IMG_Quit();
}

int Game::init_libraries() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    return -1;
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    std::cerr << IMG_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  return 0;
}

void Game::render() {
  switch (game_state) {
  case GameState::PLAYING:
    renderer.render_game(board, held_piece, mg.legal_moves);
    break;
  case GameState::PROMOTING:
    renderer.render_promotion(promoting_square, board, held_piece);
    break;
  case GameState::MENU:
    break;
  }
}

void Game::update() { ihandler.handle_input(*this); }

void Game::run() {
  mg.generate_legal_moves(board);
  while (is_running) {
    uint32_t frame_start = SDL_GetTicks();

    update();
    render();

    uint32_t frame_time = SDL_GetTicks() - frame_start;

    if (FRAME_DELAY > frame_time)
      SDL_Delay(FRAME_DELAY - frame_time);
  }
}

void Game::pick_up_piece(int mouse_x, int mouse_y) {
  held_piece.is_piece_held = true;
  held_piece.x = mouse_x;
  held_piece.y = mouse_y;
  held_piece.index = coordinates_to_index(mouse_x, mouse_y);
}
void Game::drop_piece(int mouse_x, int mouse_y) {
  held_piece.is_piece_held = false;
  int new_position_index =
      held_piece.x / 100 + (8 * (7 - (held_piece.y / 100)));
  Move move_to_make = {held_piece.index, new_position_index, 0};

  for (auto move : mg.legal_moves) {
    if (move == move_to_make) {
      if (move.is_promotion())
        promotion_query(move.to);
      else {
        board.do_move(move);
        mg.generate_legal_moves(board);
      }
      break;
    }
  }
}

void Game::move_held_piece(int mouse_x, int mouse_y) {
  held_piece.x = mouse_x;
  held_piece.y = mouse_y;
}
void Game::copy_fen_to_clipboard() {
  SDL_SetClipboardText(Fen::generate_fen(board).c_str());
}

void Game::promote(int mouse_x, int mouse_y) {
  int click_index = coordinates_to_index(mouse_x, mouse_y);

  switch ((promoting_square - click_index) / 8) {
  case 0:
    board.do_move({held_piece.index, promoting_square, FLAG_PROMOTE_QUEEN});
    break;
  case 1:
    board.do_move({held_piece.index, promoting_square, FLAG_PROMOTE_ROOK});
    break;
  case 2:
    board.do_move({held_piece.index, promoting_square, FLAG_PROMOTE_BISHOP});
    break;
  case 3:
    board.do_move({held_piece.index, promoting_square, FLAG_PROMOTE_KNIGHT});
    break;
  }

  game_state = GameState::PLAYING;
}

void Game::promotion_query(int index) {
  game_state = GameState::PROMOTING;
  promoting_square = index;
}

int Game::coordinates_to_index(int x, int y) {
  if (viewing_side == Piece::WHITE)
    return x / 100 + (8 * (7 - (y) / 100));
  else
    return x / 100 + (8 * (y / 100));
}
