#include "pyl_overloads.h"

#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Scene.h"
#include "SpeculativeContacts.h"
#include "Audible.h"

// Collision Debug Functions
Drawable ToDrawable(AABB box);
Drawable ToDrawable(Circle circ);
std::array<Drawable, 2> ToDrawable(Contact con);

int Scene::Draw() {
	auto sBind = m_Shader.ScopeBind();
	for (auto& dr : m_vDrawables) {
		mat4 PMV = m_Camera.GetMat() * dr.GetMV();
		vec4 c = dr.GetColor();
		glUniformMatrix4fv(m_Shader["u_PMV"], 1, GL_FALSE, glm::value_ptr(PMV));
		glUniform4f(m_Shader["u_Color"], c[0], c[1], c[2], c[3]);
		dr.Draw();
	}

	//for (auto& wall : m_Walls) {
	//	Drawable drWall(ToDrawable(wall));
	//	mat4 PMV = m_Camera.GetMat() * drWall.GetMV();
	//	vec4 c = drWall.GetColor();
	//	glUniformMatrix4fv(m_Shader["u_PMV"], 1, GL_FALSE, glm::value_ptr(PMV));
	//	glUniform4f(m_Shader["u_Color"], c[0], c[1], c[2], c[3]);
	//	drWall.Draw();
	//}

	for (auto& contact : m_SpeculativeContacts) {
		float nrm(-1);
		for (auto& p : contact.pos) {
			fquat rot(1, 0, 0, 0);// = getQuatFromVec2(nrm*vec2(contact.normal.y, contact.normal.x));
			
			Drawable drContact("circle.iqm", vec4(contact.normal, 1.f, 1.f),quatvec(vec3(p, -1.f), rot), 0.2f);
			mat4 PMV = m_Camera.GetMat() * drContact.GetMV();
			vec4 c = drContact.GetColor();
			glUniformMatrix4fv(m_Shader["u_PMV"], 1, GL_FALSE, glm::value_ptr(PMV));
			glUniform4f(m_Shader["u_Color"], c[0], c[1], c[2], c[3]);
			drContact.Draw();
			nrm = -nrm;
		}
	}

	return int(m_vDrawables.size());
}

int Scene::Update() {
	int nCols(0);
	float totalEnergy(0.f);
	m_SpeculativeContacts.clear();

	// Spec contacts will change all this
	for (auto& c : m_vCircles)
		c.Integrate();

	// I think there's the same # of these every time...
	Solver solve; 

	// For every circle
	for (auto it1 = m_vCircles.begin(); it1 != m_vCircles.end(); ++it1) {
		// For every unique circle pair
		for (auto it2 = it1 + 1; it2 != m_vCircles.end(); ++it2) {
			// Get circle collisions
			auto circleContacts = it1->GetClosestPoints(*it2);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), circleContacts.begin(), circleContacts.end());
		}

		// For every unique circle box pair
		for (auto& box : m_vAABB) {
			auto wallContacts = box.GetClosestPoints(*it1);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), wallContacts.begin(), wallContacts.end());
		}

		totalEnergy += it1->GetKineticEnergy();
	}

	// For every unique box pair
	for (auto it1 = m_vAABB.begin(); it1 != m_vAABB.end(); ++it1) {
		for (auto it2 = it1 + 1; it2 != m_vAABB.end(); ++it2) {
			auto boxContacts = it1->GetClosestPoints(*it2);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), boxContacts.begin(), boxContacts.end());
		}
	}

	// Conservation of energy
	//std::cout << totalEnergy << "\n" << std::endl;

	// I don't like using the bool here, so figure something else out
	solve(m_SpeculativeContacts);
	for (auto& c : m_SpeculativeContacts) {
		if (c.isColliding) {
			Entity * e1 = c.pair[0]->GetEntity();
			Entity * e2 = c.pair[1]->GetEntity();
			if (e1 && e2) {
				e1->GetPyModule().call_function("HandleCollision", e1->GetID(), e2->GetID());
				e2->GetPyModule().call_function("HandleCollision", e2->GetID(), e1->GetID());
			}
		}
	}



	//for (auto& d : m_vDrawables) 
	//	d.PyUpdate();

	for (auto& e : m_vEntities)
		e.Update();

	return nCols;
}

Scene::Scene(std::string& pyinitScript) {
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

	// Like a mini factory (no collision info for now, just circles)
	using EntInfo = std::tuple<
		vec2, // Position
		vec2, // Scale
		fquat, // Rotation
		vec4, // Color
		std::string>;

	// Now loop through all named tuples in the script
	std::vector<EntInfo> v_EntInfo;
	check(pyinitModule.get_attr("r_Entities").convert(v_EntInfo), "Getting all Entity Info");

	// Preallocate all entities
	m_vEntities.resize(v_EntInfo.size());

	// Once you have it, create individual entities and components
	for (int i = 0; i < m_vEntities.size(); i++) {
		// Get the tuple
		auto ei = v_EntInfo[i];

		// Unpack tuple
		vec3 pos(std::get<0>(ei), 0.f);
		vec3 scale(std::get<1>(ei), 1.f);
		fquat rot = std::get<2>(ei);
		vec4 color = glm::clamp(std::get<3>(ei), vec4(0), vec4(1));
		std::string pyEntModScript = std::get<4>(ei);

		// Load the python module
		auto pyEntModule = Python::Object::from_script(SCRIPT_DIR + pyEntModScript);

		// Create Entity
		Entity ent(i, this, pyEntModule);
		m_vEntities[i] = ent;

		// IQM File
		std::string iqmFile;
		check(pyEntModule.get_attr("r_IqmFile").convert(iqmFile), "Getting IqmFile from module " + pyEntModScript);

		// Sounds
		std::list<std::string> sndFiles;
		check(pyEntModule.get_attr("r_Sounds").convert(sndFiles), "Getting all sounds from module " + pyEntModScript);
		for (auto& file : sndFiles) 
			Audio::LoadSound(file);

		// Collision primitives (this will get more complicated)
		std::string colPrim;
		check(pyEntModule.get_attr("r_ColPrim").convert(colPrim), "Getting basic collision primitive from ent module");

		// Make collision resource, (assume uniform scale, used for mass and r)
		// TODO add mass, elasticity to init tuple
		if (colPrim == "AABB") {  // AABBs are assumed to be "walls" of high mass for now
			AABB box(vec2(0.f), vec2(pos), 1e10f, 1.f, vec2(scale));
			box.SetEntity(&m_vEntities[i]);
			m_vAABB.push_back(box);
		}
		else {
			Circle circ(5.f*vec2(pos), vec2(pos), 1e10f, 1.f, maxEl(scale));
			circ.SetEntity(&m_vEntities[i]);;
			m_vCircles.push_back(circ);
		}

		// Make drawable
		Drawable dr(iqmFile, color, quatvec(pos, rot), maxEl(scale));
		dr.SetEntity(&m_vEntities[i]);
		m_vDrawables.push_back(dr);
	}

	// Fix entity pointers (I hate this)
	for (auto& circle : m_vCircles)
		circle.GetEntity()->SetColCmp(&circle);
	for (auto& box : m_vAABB)
		box.GetEntity()->SetColCmp(&box);
	for (auto& drawable : m_vDrawables)
		drawable.GetEntity()->SetDrCmp(&drawable);

	// Expose in python, mapping ent ID to Exposed Entity
	for (auto& ent : m_vEntities)
		ent.GetPyModule().call_function("AddEntity", ent.GetID(), &ent);
}