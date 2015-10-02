#pragma once

#include "Scene.h"

#include <SDL.h>

#include <memory>

bool InitEverything(SDL_GLContext&, SDL_Window*&, std::unique_ptr<Scene>&);
bool InitSDL();
bool InitGL(SDL_GLContext&, SDL_Window*&);
bool InitSound();
bool InitPython();
bool InitScene(std::unique_ptr<Scene>& pScene);