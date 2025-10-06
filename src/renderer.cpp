#include "renderer.h"
#include "board.h"
#include "piece.h"
#include <SDL.h>
#include <SDL_image.h>
#include <bit>
#include <iostream>

enum piece_texture_index {
  WHITE_KING,
  WHITE_QUEEN,
  WHITE_ROOK,
  WHITE_BISHOP,
  WHITE_KNIGHT,
  WHITE_PAWN,

  BLACK_KING,
  BLACK_QUEEN,
  BLACK_ROOK,
  BLACK_BISHOP,
  BLACK_KNIGHT,
  BLACK_PAWN,
};

void Renderer::init(SDL_Window *window) {
  sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!sdl_renderer) {
    std::cerr << SDL_GetError() << std::endl;
    SDL_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(sdl_renderer);
    exit(-1);
  }

  load_piece_textures();
}

Renderer::~Renderer() {
  for (int i = 0; i < 12; i++) {
    if (piece_textures[i])
      SDL_DestroyTexture(piece_textures[i]);
  }

  if (sdl_renderer)
    SDL_DestroyRenderer(sdl_renderer);
}

void Renderer::load_piece_texture(int index, const char *path) {
  piece_textures[index] = IMG_LoadTexture(sdl_renderer, path);

  if (piece_textures[index])
    std::cerr << "Loaded piece at path " << path << " at index " << index
              << std::endl;
  else {
    std::cerr << "Failed to load piece at path " << path << " at index "
              << index << std::endl;

    exit(-1);
  }
}

void Renderer::draw_bitboard(bitboard bb) const {
  while (bb) {
    int index = std::countr_zero(bb);
    bb &= bb - 1;

    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

    rect.x = (index % 8) * TILE_SIZE;
    rect.y = (7 - (index / 8)) * TILE_SIZE;

    SDL_RenderFillRect(sdl_renderer, &rect);
  }
}

void Renderer::load_piece_textures() {
  load_piece_texture(WHITE_KING, "res/100x100/wk.png");
  load_piece_texture(WHITE_QUEEN, "res/100x100/wq.png");
  load_piece_texture(WHITE_ROOK, "res/100x100/wr.png");
  load_piece_texture(WHITE_BISHOP, "res/100x100/wb.png");
  load_piece_texture(WHITE_KNIGHT, "res/100x100/wn.png");
  load_piece_texture(WHITE_PAWN, "res/100x100/wp.png");

  load_piece_texture(BLACK_KING, "res/100x100/bk.png");
  load_piece_texture(BLACK_QUEEN, "res/100x100/bq.png");
  load_piece_texture(BLACK_ROOK, "res/100x100/br.png");
  load_piece_texture(BLACK_BISHOP, "res/100x100/bb.png");
  load_piece_texture(BLACK_KNIGHT, "res/100x100/bn.png");
  load_piece_texture(BLACK_PAWN, "res/100x100/bp.png");
}

void Renderer::render(Board &board, HeldPiece *held_piece,
                      const std::vector<Move> &legal_moves) const {
  SDL_RenderClear(sdl_renderer);

  draw_board_tiles();
  draw_legal_moves(legal_moves, held_piece);
  // SDL_SetRenderDrawColor(sdl_renderer, 0, 255, 0, 255);
  // draw_bitboard(board.black_bitboard);
  // SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 255, 255);
  // draw_bitboard(board.white_bitboard);
  draw_pieces(held_piece);
  draw_held_piece(held_piece);

  SDL_RenderPresent(sdl_renderer);
}

void Renderer::draw_board_tiles() const {
  SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

  bool white = true;
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      rect.x = x * TILE_SIZE;
      rect.y = y * TILE_SIZE;

      if (white)
        SDL_SetRenderDrawColor(sdl_renderer, 240, 217, 181, 255); // Light
      else
        SDL_SetRenderDrawColor(sdl_renderer, 181, 136, 99, 255); // Dark

      SDL_RenderFillRect(sdl_renderer, &rect);
      white = !white;
    }
    white = !white;
  }
}

int Renderer::get_piece_texture_index(int piece) const {

  int texture_index = -1;
  texture_index += piece & Piece::type_mask;
  if ((piece & Piece::color_mask) != Piece::WHITE)
    texture_index += 6;

  return texture_index;
}

void Renderer::draw_pieces(const HeldPiece *held_piece) const {

  SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

  for (int i = 0; i < Board::SQUARES; i++) {
    if (held_piece->is_piece_held && i == held_piece->index)
      continue;

    if ((_board.piece_at(i) & Piece::type_mask) == Piece::EMPTY)
      continue;

    rect.x = (i % 8) * TILE_SIZE;
    rect.y = (7 - (i / 8)) * TILE_SIZE;

    int texture_index = get_piece_texture_index(_board.piece_at(i));

    if (texture_index != -1) {
      SDL_RenderCopy(sdl_renderer, piece_textures[texture_index], nullptr,
                     &rect);
    }
  }
}

void Renderer::draw_held_piece(HeldPiece *held_piece) const {
  if (!held_piece->is_piece_held)
    return;

  int piece = _board.piece_at(held_piece->index);
  if (!piece)
    return;

  SDL_Rect rect = {held_piece->x - (TILE_SIZE / 2),
                   held_piece->y - (TILE_SIZE / 2), TILE_SIZE, TILE_SIZE};

  int texture_index = get_piece_texture_index(piece);
  SDL_Texture *texture = piece_textures[texture_index];

  SDL_RenderCopy(sdl_renderer, texture, nullptr, &rect);
}

void Renderer::draw_legal_moves(const std::vector<Move> &legal_moves,
                                const HeldPiece *held_piece) const {
  if (!held_piece->is_piece_held)
    return;
  SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

  SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);

  for (int i = 0; i < legal_moves.size(); i++) {
    if (legal_moves[i].from == held_piece->index) {
      rect.x = legal_moves[i].to % 8 * TILE_SIZE;
      rect.y = (7 - (legal_moves[i].to / 8)) * TILE_SIZE;

      SDL_RenderFillRect(sdl_renderer, &rect);
    }
  }
}
