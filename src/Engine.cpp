#include "Engine.h"
#include <iostream>

Engine engine;

Engine::Engine() {
  if(init())
    std::cout << "\033[32m[SDL] Successfully initialized rendering backend.\033[0m\n";
  else
    std::cerr << "\033[31m[SDL] Failed to initialize!\033[0m\n";
}

Engine::~Engine() {
  close();
}

bool Engine::init() {
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    return false;
  else 
    return true;
}

void Engine::close() {
  SDL_Quit();
}
