#include "Engine.h"
#include "Output.h"
#include <iostream>

Engine engine;

Engine::Engine() {
  if(init())
    std::cout << Output::Colors::GREEN << "[SDL] Successfully initialized rendering backend." << Output::Colors::END << '\n';
  else
    std::cerr << Output::Colors::RED << "[SDL] Failed to initialize!" << Output::Colors::END << '\n';
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
