#pragma once

#include "Camera.h"
#include "Shader.h"

#include <vector>

class Scene
{
public:
    // Constructors
	Scene();
	void Init(std::string XmlSource);
    
    // Render function
	int Draw();
    
private:
	Shader m_Shader; // May be a container soon
	Camera m_Camera;
	std::string m_XmlSource;
};

