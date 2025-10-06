#pragma once

#include "board.h"
#include "moves.h"
#include <SDL.h>
#include <vector>

constexpr int TILE_SIZE = 100;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

class Renderer {
public:
  explicit Renderer(Board &board) : sdl_renderer(nullptr), _board(board) {}

  ~Renderer();

  void init(SDL_Window *window);

  void render(Board &board, HeldPiece *held_piece,
              const std::vector<Move> &legal_moves) const;
  void load_piece_textures();

private:
  SDL_Renderer *sdl_renderer;
  SDL_Texture *piece_textures[12]{};
  Board &_board;

  void load_piece_texture(int index, const char *path);
  int get_piece_texture_index(int piece) const;

  void draw_board_tiles() const;
  void draw_bitboard(bitboard bb) const;
  void draw_pieces(const HeldPiece *held_piece) const;
  void draw_held_piece(HeldPiece *held_piece) const;
  void draw_legal_moves(const std::vector<Move> &legal_moves,
                        const HeldPiece *held_piece) const;
};
