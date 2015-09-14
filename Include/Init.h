#pragma once

#include "Scene.h"

#include <memory>

bool InitGL();
bool InitPython();
bool InitScene(std::unique_ptr<Scene>& pScene);