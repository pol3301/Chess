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

int Renderer::init() {
  window =
      SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << SDL_GetError() << std::endl;
    return -1;
  }

  sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!sdl_renderer) {
    std::cerr << SDL_GetError() << std::endl;
    return -1;
  }

  if (load_piece_textures() != 0)
    return -1;
  return 0;
}

Renderer::~Renderer() {
  if (sdl_renderer)
    SDL_DestroyRenderer(sdl_renderer);

  if (window)
    SDL_DestroyWindow(window);
}

int Renderer::load_piece_texture(int index, const char *path) {
  piece_textures[index].reset(IMG_LoadTexture(sdl_renderer, path));

  if (piece_textures[index])
    std::cout << "Loaded piece at path " << path << " at index " << index
              << std::endl;
  else {
    std::cerr << "Failed to load piece at path " << path << " at index "
              << index << std::endl;

    return -1;
  }

  return 0;
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

struct PieceTextureInfo {
  enum piece_texture_index index;
  const char *path;
};

int Renderer::load_piece_textures() {

  const std::array<PieceTextureInfo, 12> textures_to_load = {{
      {WHITE_KING, "res/100x100/wk.png"},
      {WHITE_QUEEN, "res/100x100/wq.png"},
      {WHITE_ROOK, "res/100x100/wr.png"},
      {WHITE_BISHOP, "res/100x100/wb.png"},
      {WHITE_KNIGHT, "res/100x100/wn.png"},
      {WHITE_PAWN, "res/100x100/wp.png"},
      {BLACK_KING, "res/100x100/bk.png"},
      {BLACK_QUEEN, "res/100x100/bq.png"},
      {BLACK_ROOK, "res/100x100/br.png"},
      {BLACK_BISHOP, "res/100x100/bb.png"},
      {BLACK_KNIGHT, "res/100x100/bn.png"},
      {BLACK_PAWN, "res/100x100/bp.png"},
  }};

  for (const auto texture : textures_to_load) {
    if (load_piece_texture(texture.index, texture.path)) {
      std::cerr << "Failed to load texture at index: " << texture.index
                << "from path: " << texture.path;
      return -1;
    }
  }
  return 0;
}

void Renderer::render(Board &board, HeldPiece &held_piece,
                      const std::vector<Move> &legal_moves) const {
  SDL_RenderClear(sdl_renderer);

  draw_board_tiles();
  draw_legal_moves(legal_moves, held_piece);
  // SDL_SetRenderDrawColor(sdl_renderer, 0, 255, 0, 255);
  // draw_bitboard(board.black_bitboard);
  // SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 255, 255);
  // draw_bitboard(board.white_bitboard);
  draw_pieces(held_piece, board);
  // draw_promotion_box(56);
  draw_held_piece(held_piece, board);

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
  texture_index += Piece::type(piece);
  if (Piece::color(piece) != Piece::WHITE)
    texture_index += 6;

  if (texture_index == -1)
    std::cerr << "Error retrieved non valid piece texture" << std::endl;

  return texture_index;
}

void Renderer::draw_pieces(const HeldPiece &held_piece,
                           const Board &board) const {

  SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

  for (int i = 0; i < Board::SQUARES; i++) {
    if (held_piece.is_piece_held && i == held_piece.index)
      continue;

    if (Piece::type(board.piece_at(i)) == Piece::EMPTY)
      continue;

    rect.x = (i % 8) * TILE_SIZE;
    rect.y = (7 - (i / 8)) * TILE_SIZE;

    int texture_index = get_piece_texture_index(board.piece_at(i));

    if (texture_index != -1) {
      SDL_RenderCopy(sdl_renderer, piece_textures[texture_index].get(), nullptr,
                     &rect);
    }
  }
}

void Renderer::draw_held_piece(const HeldPiece &held_piece,
                               const Board board) const {
  if (!held_piece.is_piece_held)
    return;

  int piece = board.piece_at(held_piece.index);
  if (!piece)
    return;

  SDL_Rect rect = {held_piece.x - (TILE_SIZE / 2),
                   held_piece.y - (TILE_SIZE / 2), TILE_SIZE, TILE_SIZE};

  int texture_index = get_piece_texture_index(piece);
  if (texture_index == -1)
    return;
  SDL_Texture *texture = piece_textures[texture_index].get();

  SDL_RenderCopy(sdl_renderer, texture, nullptr, &rect);
}

void Renderer::draw_legal_moves(const std::vector<Move> &legal_moves,
                                const HeldPiece &held_piece) const {
  if (!held_piece.is_piece_held)
    return;

  SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};

  SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);

  for (int i = 0; i < legal_moves.size(); i++) {
    if (legal_moves[i].from == held_piece.index) {
      rect.x = legal_moves[i].to % 8 * TILE_SIZE;
      rect.y = (7 - (legal_moves[i].to / 8)) * TILE_SIZE;

      SDL_RenderFillRect(sdl_renderer, &rect);
    }
  }
}

int Renderer::draw_promotion_box(int square) const {

  int x = (square % 8) * TILE_SIZE;
  int y = (7 - (square / 8)) * TILE_SIZE;

  SDL_Rect queen_rect = {x, y, TILE_SIZE, TILE_SIZE};
  SDL_Rect rook_rect = {x, y + TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_Rect bishop_rect = {x, y + 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_Rect knight_rect = {x, y + 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE};

  SDL_Rect background{1, 2, 3, 4};

  SDL_RenderCopy(sdl_renderer, piece_textures[WHITE_QUEEN].get(), nullptr,
                 &queen_rect);
  SDL_RenderCopy(sdl_renderer, piece_textures[WHITE_ROOK].get(), nullptr,
                 &rook_rect);
  SDL_RenderCopy(sdl_renderer, piece_textures[WHITE_BISHOP].get(), nullptr,
                 &bishop_rect);
  SDL_RenderCopy(sdl_renderer, piece_textures[WHITE_KNIGHT].get(), nullptr,
                 &knight_rect);

  return 0;
}
