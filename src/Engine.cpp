#include "Engine.h"
#include <iostream>

Engine engine;

Engine::Engine() {
  if(init())
    std::cout << "SDL initialized successfully!\n";
  else
    std::cerr << "SDL failed to initialize!\n";
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
