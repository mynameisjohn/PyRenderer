#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Drawable.h"
#include "RigidBody.h"
#include "SpeculativeContacts.h"

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
	
	// Ideally these would be planes
	std::array<AABB, 4> m_Walls;

	std::vector<Entity> m_vEntities;
	std::vector<Drawable> m_vDrawables;
	std::vector<Circle> m_vCircles;
	std::list<Contact> m_SpeculativeContacts; // no need to be a list, will be (n^2+n)/2
};

