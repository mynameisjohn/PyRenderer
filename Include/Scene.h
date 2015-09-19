#pragma once

#include "Camera.h"
#include "Shader.h"
#include "PyEntity.h"
#include "Drawable.h"
#include "RigidBody.h"

#include <pyliason.h>

#include <vector>

class Scene
{
public:
    // Constructors
	Scene(std::string& pyInitModule);
    
    // Render function
	int Draw();
	int Update();
    
	// 
private:
	Shader m_Shader; // May be a container soon
	Camera m_Camera;
	
	std::vector<Entity> m_vEntities;
	std::vector<Drawable> m_vDrawables;
	std::vector<Circle> m_vCircles;

	std::map<std::string, Python::Object> m_PyObjCache;
};

