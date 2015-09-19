#pragma once

#include "Scene.h"

#include <SDL.h>

#include <memory>

bool InitGL(SDL_GLContext&, SDL_Window*&);
bool InitPython();
bool InitScene(std::unique_ptr<Scene>& pScene);