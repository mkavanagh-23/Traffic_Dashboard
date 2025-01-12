#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>

class Engine {
public:
  Engine();
  ~Engine();
public:
  bool init();
  void close();
};

extern Engine engine;

#endif
