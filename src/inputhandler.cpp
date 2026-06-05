#include "inputhandler.h"
#include "game.h"

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <iostream>

void InputHandler::handle_input(Game &g) {
  SDL_Event e;
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);

  while (SDL_PollEvent(&e)) {
    // switch (e.type) {
    // case SDL_QUIT:
    //   g.quit();
    //
    // case SDL_MOUSEBUTTONDOWN:
    //   g.pick_up_piece(mouse_x, mouse_y);
    //   break;
    //
    // case SDL_MOUSEBUTTONUP:
    //   g.drop_piece(mouse_x, mouse_y);
    //   break;
    //
    // case SDL_MOUSEMOTION:
    //   g.move_held_piece(mouse_x, mouse_y);
    //   break;
    //
    // case SDL_KEYDOWN:
    //   SDL_Keycode key_pressed = e.key.keysym.sym;
    //   Uint16 key_mod = e.key.keysym.mod;
    //
    //   if (key_mod & KMOD_CTRL) {
    //     if (key_pressed == SDLK_z)
    //       g.undo_move();
    //     if (key_pressed == SDLK_c)
    //       g.copy_fen_to_clipboard();
    //     if (key_pressed == SDLK_v) {
    //       char *clipboard_raw = SDL_GetClipboardText();
    //       if (clipboard_raw != nullptr && *clipboard_raw != '\0') {
    //         std::string clipboard = clipboard_raw;
    //         if (!g.paste_fen(clipboard))
    //           std::cout << "Pasetd fen: " << clipboard << std::endl;
    //         else
    //           std::cerr << "Couldn't paste fen" << std::endl;
    //       } else
    //         std::cerr << "Coudln't read clipboard" << std::endl;
    //       SDL_free(clipboard_raw);
    //     }
    //   }
    // }

    if ((e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) ||
        (e.type == SDL_QUIT)) {
      g.quit();
      break;
    }

    switch (g.get_state()) {
    case GameState::PLAYING:
      handle_playing_input(g, e, mouse_x, mouse_y);
      break;
    case GameState::PROMOTING:
    case GameState::MENU:
      break;
    }
  }
}

void InputHandler::handle_playing_input(Game &g, SDL_Event &e, int mouse_x,
                                        int mouse_y) {
  switch (e.type) {
  case SDL_MOUSEBUTTONDOWN:
    g.pick_up_piece(mouse_x, mouse_y);
    break;

  case SDL_MOUSEBUTTONUP:
    g.drop_piece(mouse_x, mouse_y);
    break;

  case SDL_MOUSEMOTION:
    g.move_held_piece(mouse_x, mouse_y);
    break;
  case SDL_KEYDOWN:
    SDL_Keycode key_pressed = e.key.keysym.sym;
    Uint16 key_mod = e.key.keysym.mod;

    if (key_mod & KMOD_CTRL) {
      if (key_pressed == SDLK_z)
        g.undo_move();
      if (key_pressed == SDLK_c)
        g.copy_fen_to_clipboard();
      if (key_pressed == SDLK_v) {
        char *clipboard_raw = SDL_GetClipboardText();
        if (clipboard_raw != nullptr && *clipboard_raw != '\0') {
          std::string clipboard = clipboard_raw;
          if (!g.paste_fen(clipboard))
            std::cout << "Pasetd fen: " << clipboard << std::endl;
          else
            std::cerr << "Couldn't paste fen" << std::endl;
        } else
          std::cerr << "Coudln't read clipboard" << std::endl;
        SDL_free(clipboard_raw);
      }
    } else {
    }
  }
}

void InputHandler::handle_promoting_input(Game &g, SDL_Event &e, int mouse_x,
                                          int mouse_y) {
  if (e.type == SDL_MOUSEBUTTONUP) {
  }
}
