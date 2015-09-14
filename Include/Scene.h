#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Entity.h"
#include "Drawable.h"

#include <vector>

class Scene
{
public:
    // Constructors
	Scene(std::string& pyInitModule);
    
    // Render function
	int Draw();
    
	// 
private:
	Shader m_Shader; // May be a container soon
	Camera m_Camera;
	
	std::vector<Entity> m_vEntities;
	std::vector<Drawable> m_vDrawables;
};

