#pragma once

#include "board.h"
#include "moves.h"
#include <SDL.h>
#include <SDL_render.h>
#include <cstddef>
#include <memory>
#include <vector>

struct HeldPiece;

struct TextureDeleter {
  void operator()(SDL_Texture *texture) const {
    if (texture) {
      SDL_DestroyTexture(texture);
    }
  }
};

using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;

constexpr int TILE_SIZE = 100;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

class Renderer {
public:
  explicit Renderer() : sdl_renderer(nullptr) {}

  ~Renderer();

  int init();

  void render_game(Board &board, HeldPiece &held_piece,
                   const std::vector<Move> &legal_moves) const;

private:
  SDL_Window *window;
  SDL_Renderer *sdl_renderer;
  TexturePtr piece_textures[12];

  int load_piece_texture(int index, const char *path);
  int get_piece_texture_index(int piece) const;

  int load_piece_textures();
  void draw_board_tiles() const;
  void draw_bitboard(bitboard bb) const;
  void draw_pieces(const HeldPiece &held_piece, const Board &board) const;
  void draw_held_piece(const HeldPiece &held_piece, const Board board) const;
  void draw_legal_moves(const std::vector<Move> &legal_moves,
                        const HeldPiece &held_piece) const;
  int draw_promotion_box(int square) const;
};
