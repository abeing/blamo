#include <iostream>
#include <time.h>
#include <cassert>
#include <vector>
#include <stdlib.h>
#include <SDL2/SDL.h>

void logSdlError(std::ostream &os, const std::string &msg)
{
  os << msg << " error: " << SDL_GetError() << std::endl;
}

void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *)p = pixel;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      } else {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;

    case 4:
      *(Uint32 *)p = pixel;
      break;
  }
}

/**
 * Unfairly toss a coin.
 *
 * @param trueProbability The chance of the unfair coin coming up true.
 * @return true or false, with probability determined by trueProbability
 */
bool unfairCoin(double trueProbability) {
  return (rand()/(double)RAND_MAX) < trueProbability;
}

/**
 * Generates the battle field terrain. The terrain is a series heights, each no heigher than the maximumHeight.
 *
 * @param width the width of the terrain to generate; determines the length of the resulting vector.
 * @param height the maximum height of any element of the curve
 * @param variance the "jaggedness" of the curve
 * @return a vector of terrain heights
 */
std::vector<int> generateTerrainCurve(unsigned width, unsigned maximumHeight, double variance)
{
  std::vector<int> curve;
  int previousHeight = 200;
  for (int i = 0; i < width; ++i) {
    int bump = unfairCoin(0.5) ? 1 : (unfairCoin(0.5) ? 2 : 3);
    int nextHeight = unfairCoin(variance) ? previousHeight + bump : previousHeight - bump;
    curve.push_back(nextHeight);
    previousHeight = nextHeight;
  }
  return curve;
}

int main(int argc, char **argv)
{
  srand(time(0));
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    logSdlError(std::cerr, "SDL_Init");
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Blamo", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    logSdlError(std::cerr, "SDL_CreateWindow");
    return 2;
  }
  // Create a surface and directly access the pixels
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    logSdlError(std::cerr, "SDL_CreateRenderer");
    return 3;
  }
  Uint32 format = SDL_GetWindowPixelFormat(window);
  int bpp;
  Uint32 Rmask, Gmask, Bmask, Amask = 0;
  SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
  SDL_Surface* field = SDL_CreateRGBSurface(0, 640, 480, 32, Rmask, Gmask, Bmask, Amask);
  Uint32 ground = SDL_MapRGB(field->format, 0, 255, 0);

  SDL_LockSurface(field);
  std::vector<int> curve = generateTerrainCurve(640, 480, 0.5);
  assert(curve.size() == 640);
  for (int i = 0; i < 640; ++i) {
    for (int j = curve[i]; j < 480; j++) {
      putPixel(field, i, j, ground);
    }
  }
  // putPixel(field, 10, 10, ground);
  SDL_UnlockSurface(field);

  SDL_Texture *texture = nullptr;
  texture = SDL_CreateTextureFromSurface(renderer, field);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_Delay(2000);

  SDL_FreeSurface(field);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
