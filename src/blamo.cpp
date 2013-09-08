#include <iostream>
#include <SDL2/SDL.h>

void logSdlError(std::ostream &os, const std::string &msg)
{
  os << msg << " error: " << SDL_GetError() << std::endl;
}

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    logSdlError(std::cerr, "SDL_Init");
    return 1;
  }
}
