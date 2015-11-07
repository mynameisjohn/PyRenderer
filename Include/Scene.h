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
    
    Python::Object GetPyModule();
	//
private:
	Shader m_Shader; // May be a container soon
	Camera m_Camera;

    bool m_bShowContacts;
    
	std::vector<Entity> m_vEntities;
	std::vector<Drawable> m_vDrawables;
	std::vector<Circle> m_vCircles;
	std::vector<AABB> m_vAABB;
	std::vector<OBB> m_vOBB;
	std::list<Contact> m_SpeculativeContacts; // no need to be a list, will be (n^2+n)/2
    Solver m_ContactSolver;
    
    Python::Object m_MainPyModule;
};

