#include "pyl_overloads.h"

#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Scene.h"
#include "SpeculativeContacts.h"

int Scene::Draw() {
	auto sBind = m_Shader.ScopeBind();
	for (auto& dr : m_vDrawables) {
		mat4 PMV = m_Camera.GetMat() * dr.GetMV();
		vec4 c = dr.GetColor();
		glUniformMatrix4fv(m_Shader["u_PMV"], 1, GL_FALSE, glm::value_ptr(PMV));
		glUniform4f(m_Shader["u_Color"], c[0], c[1], c[2], c[3]);
		dr.Draw();
		//m_PyObjCache.begin()->second.call_function("sayHello", &m_Camera);
	}

	std::list<Drawable> dr_ContactPoints;
	for (auto& contact : m_SpeculativeContacts) {
		for (auto& p : contact.pos) {
			glm::mat4 MV(glm::translate(vec3(contact.pos[0], 0.f))*glm::scale(vec3(0.5f)));
			dr_ContactPoints.emplace_back("circle.iqm", vec4(1, 0, 1, 1), MV);
		}
	}

	m_SpeculativeContacts.clear();

	return int(m_vDrawables.size());
}

int Scene::Update() {
	int nCols(0);

	// I think there's the same # of these every time...
	Solver solve; 

	for (auto it1 = m_vCircles.begin(); it1 != m_vCircles.end(); ++it1) {
		for (auto it2 = it1 + 1; it2 != m_vCircles.end(); ++it2) {
			// Get circle collisions
			auto circleContacts = it1->GetClosestPoints(*it2);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), circleContacts.begin(), circleContacts.end());
		}

		// Get collisions with wall
		for (auto& wall : m_Walls) {
			auto wallContacts = wall.GetClosestPoints(*it1);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), wallContacts.begin(), wallContacts.end());
		}
	}

	solve(m_SpeculativeContacts);

	// Spec contacts will change all this
	for (auto& c : m_vCircles) {
		c.Integrate();
		c.PyUpdate();
	}

	for (auto& d : m_vDrawables) 
		d.PyUpdate();

	for (auto& e : m_vEntities)
		e.Update();

	return nCols;
}

Scene::Scene(std::string& pyinitScript) :
	m_Walls({AABB(), AABB(), AABB(), AABB()})
{
	auto check = [](bool cond, std::string msg = "") {
		if (!msg.empty())
			std::cout << msg << "----" << (cond ? " succeeded! " : " failed! ") << std::endl;
		assert(cond);
	};

	// Convert the relative python path to the absolute, load module
	std::string initStrPath = FixBackslash(RelPathToAbs(SCRIPT_DIR + pyinitScript));
	auto pyinitModule = Python::Object::from_script(initStrPath);

	// Set up the shader
	std::map<std::string, std::string> shaderInfo;
	check(pyinitModule.get_attr("r_ShaderSrc").convert(shaderInfo), "Getting shader info from pyinit module " + pyinitScript);
	m_Shader = Shader(shaderInfo["vert"], shaderInfo["frag"], SHADER_DIR);

	// Get position handle
	auto sBind = m_Shader.ScopeBind();
	GLint posHandle = m_Shader[shaderInfo["Position"]];
	Drawable::SetPosHandle(posHandle);

	// Initialize Camera
	pyinitModule.call_function("InitCamera", &m_Camera);

	// These should match the camera
	vec4 wB; // vec4(x,y,w,h)
	pyinitModule.call_function("GetWalls").convert(wB);
	
	// The walls are actually four boxes
	m_Walls[0] = AABB(vec2(0), FLT_MAX/2.f, 1.f, wB.x - wB.z, wB.y, wB.z, wB.w); // (x-w,y,w,h)
	m_Walls[1] = AABB(vec2(0), FLT_MAX / 2.f, 1.f, wB.x, wB.y - wB.w, wB.z, wB.w); // (x,y-h,w,h)
	m_Walls[2] = AABB(vec2(0), FLT_MAX / 2.f, 1.f, wB.x + wB.z, wB.y, wB.z, wB.w); // (x+w,y,w,h)
	m_Walls[3] = AABB(vec2(0), FLT_MAX / 2.f, 1.f, wB.x, wB.y + wB.w, wB.z, wB.w); // (x, y+h, w, h)

	// Make drawables for them, because why not
	m_vDrawables.emplace_back("quad.iqm", vec4(1), glm::translate(vec3(-wB.z, 0.f, 0.f))*glm::scale(vec3(wB.z, wB.w, 1.f)));
	m_vDrawables.emplace_back("quad.iqm", vec4(1), glm::translate(vec3(wB.z, 0.f, 0.f))*glm::scale(vec3(wB.z, wB.w, 1.f)));
	m_vDrawables.emplace_back("quad.iqm", vec4(1), glm::translate(vec3(0.f, wB.w, 0.f))*glm::scale(vec3(wB.z, wB.w, 1.f)));
	m_vDrawables.emplace_back("quad.iqm", vec4(1), glm::translate(vec3(0.f, -wB.w, 0.f))*glm::scale(vec3(wB.z, wB.w, 1.f)));

	// Like a mini factory (no collision info for now, just circles)
	using EntInfo = std::tuple<vec3, vec3, fquat, vec4, std::string>;

	// Now loop through all named tuples in the script
	std::vector<EntInfo> v_EntInfo;
	check(pyinitModule.get_attr("r_Entities").convert(v_EntInfo), "Getting all Entity Info");

	// Precreate all entities, which may be a bad idea
	m_vEntities.resize(v_EntInfo.size());
	m_vCircles.resize(v_EntInfo.size());
	m_vDrawables.resize(4 + v_EntInfo.size());

	// Once you have it, create individual entities and components
	for (int i = 0; i < m_vEntities.size(); i++) {
		// Get the tuple
		auto ei = v_EntInfo[i];

		// Unpack tuple
		vec3 pos = std::get<0>(ei);
		vec3 scale = std::get<1>(ei);
		fquat rot = std::get<2>(ei);
		vec4 color = glm::clamp(std::get<3>(ei), vec4(0), vec4(1));
		std::string pyEntModScript = std::get<4>(ei);

		// Load the python module
		auto pyEntModule = Python::Object::from_script(SCRIPT_DIR + pyEntModScript);

		// MV Transform
		mat4 MV = glm::translate(pos) * glm::mat4_cast(rot) * glm::scale(scale);

		// IQM File
		std::string iqmFile;
		check(pyEntModule.get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module " + pyEntModScript);

		// Make drawable
		Drawable dr(iqmFile, color, MV, &m_vEntities[i]);

		// Get collision info
		float radius = scale[0]; // Assume uniform scale for now...
		vec2 c(pos);

		// Reinitialize vector objects
		m_vDrawables[i] = dr;
		m_vCircles[i] = Circle(vec2(-pos), vec2(pos), 1.f, 1.f, scale[0], &m_vEntities[i]);

		// Get the pointers (this has to change)
		Drawable * drPtr = &m_vDrawables[i];
		Circle * cPtr = &m_vCircles[i];

		// ID is len of the list in python
		int uID; 
		pyEntModule.call_function("AddEntity", &m_vEntities[i]).convert(uID);
		
		// Reinitialize entity
		m_vEntities[i] = Entity(uID, cPtr, drPtr);
		m_vEntities[i].m_PyModule = pyEntModule;
	}
}
