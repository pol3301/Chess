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

void Game::update() {
  // int x, y;
  // Move move;
  // SDL_Keycode key_pressed;
  // Uint16 key_mod;
  // SDL_Event e;
  //
  // std::vector<Move>::iterator moveIndex;
  //
  // SDL_GetMouseState(&held_piece.x, &held_piece.y);
  //
  // x = held_piece.x / 100;
  // y = held_piece.y / 100;
  //
  // y = 7 - y;
  //
  // while (SDL_PollEvent(&e)) {
  //   switch (e.type) {
  //   case SDL_QUIT:
  //     is_running = false;
  //
  //   case SDL_MOUSEBUTTONDOWN:
  //     held_piece.index = y * 8 + x;
  //     held_piece.is_piece_held = true;
  //     break;
  //
  //   case SDL_MOUSEBUTTONUP:
  //     held_piece.is_piece_held = false;
  //     move = {held_piece.index, y * 8 + x, 0};
  //
  //     moveIndex = std::find(mg.legal_moves.begin(), mg.legal_moves.end(),
  //     move); if (moveIndex != mg.legal_moves.end()) {
  //       board.do_move(*moveIndex);
  //       mg.generate_legal_moves(board);
  //     }
  //
  //     break;
  //
  //   case SDL_MOUSEMOTION:
  //     SDL_GetMouseState(&held_piece.x, &held_piece.y);
  //     break;
  //
  //   case SDL_KEYDOWN:
  //     key_pressed = e.key.keysym.sym;
  //     key_mod = e.key.keysym.mod;
  //
  //     if (key_mod & KMOD_CTRL) {
  //       switch (key_pressed) {
  //       case SDLK_z:
  //         board.undo_move();
  //         mg.generate_legal_moves(board);
  //         break;
  //       case SDLK_c:
  //         SDL_SetClipboardText(Fen::generate_fen(board).c_str());
  //         break;
  //       case SDLK_v:
  //         char *clipboard_raw = SDL_GetClipboardText();
  //         if (clipboard_raw != nullptr && *clipboard_raw != '\0') {
  //           std::string clipboard = clipboard_raw;
  //           std::cout << clipboard << std::endl;
  //           board = Board(clipboard);
  //         }
  //         SDL_free(clipboard_raw);
  //
  //         mg.generate_legal_moves(board);
  //         break;
  //       }
  //     } else {
  //       switch (key_pressed) {
  //       case SDLK_ESCAPE:
  //         is_running = false;
  //         break;
  //       case SDLK_n:
  //         board.new_turn();
  //         mg.generate_legal_moves(board);
  //         break;
  //       }
  //     }
  //
  //   default:
  //     break;
  //   }
  // }
  ihandler.handle_input(*this);
}

void Game::run() {
  mg.generate_legal_moves(board);
  while (is_running) {
    uint32_t frame_start = SDL_GetTicks();

    update();
    renderer.render_game(board, held_piece, mg.legal_moves);

    uint32_t frame_time = SDL_GetTicks() - frame_start;

    if (FRAME_DELAY > frame_time)
      SDL_Delay(FRAME_DELAY - frame_time);
  }
}

void Game::pick_up_piece(int mouse_x, int mouse_y) {
  held_piece.is_piece_held = true;
  held_piece.x = mouse_x;
  held_piece.y = mouse_y;
  held_piece.index = mouse_x / 100 + (8 * (7 - (mouse_y) / 100));
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

// void Game::promotion_query(int index) { game_state = GameState::PROMOTING; }
