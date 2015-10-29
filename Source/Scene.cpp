#include "pyl_overloads.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    if (m_bShowContacts){
        for (auto& contact : m_SpeculativeContacts) {
            for (auto& p : contact.pos) {
                fquat rot(1, 0, 0, 0);
                
                //std::cout << contact.normal << ", " << 0.5f * (contact.normal + vec2(1)) << std::endl;
                
                // Contact color is normal remapped to 0,1
                const vec2 v1(1);
                float rndBlue = uint32_t(rand()) / float(RAND_MAX);
                vec4 ctColor = vec4(remap(contact.normal, -v1, v1, vec2(), v1), rndBlue, 1.f);
                quatvec ctTr = quatvec(vec3(p, -1.f), rot);
                vec2 ctScl = vec2(0.2f);
                
                Drawable drContact("circle.iqm", ctColor, ctTr, ctScl);
                mat4 PMV = m_Camera.GetMat() * drContact.GetMV();
                vec4 c = drContact.GetColor();
                glUniformMatrix4fv(m_Shader["u_PMV"], 1, GL_FALSE, glm::value_ptr(PMV));
                glUniform4f(m_Shader["u_Color"], c[0], c[1], c[2], c[3]);
                drContact.Draw();
            }
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

	for (auto& b : m_vAABB)
		b.Integrate();

	for (auto& b : m_vOBB)
		b.Integrate();
    
//    for (auto& b : m_vOBB)
//        b.V.y -= 1.f;
    
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

		// for every unique circle OBB pair
		for (auto& obb : m_vOBB) {
			auto bContacts = obb.GetClosestPoints(*it1);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), bContacts.begin(), bContacts.end());
		}

		totalEnergy += it1->GetKineticEnergy();
	}

	// For every unique box pair
	for (auto it1 = m_vAABB.begin(); it1 != m_vAABB.end(); ++it1) {
		for (auto it2 = it1 + 1; it2 != m_vAABB.end(); ++it2) {
			auto boxContacts = it1->GetClosestPoints(*it2);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), boxContacts.begin(), boxContacts.end());
		}

		// for every AABB - OBB pair
		for (auto& obb : m_vOBB) {
			auto bContacts = obb.GetClosestPoints(*it1);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), bContacts.begin(), bContacts.end());
		}

		totalEnergy += it1->GetKineticEnergy();
	}

	// For every unique OBB pair
	for (auto it1 = m_vOBB.begin(); it1 != m_vOBB.end(); ++it1) {
		for (auto it2 = it1 + 1; it2 != m_vOBB.end(); ++it2) {
			auto boxContacts = it1->GetClosestPoints(*it2);
			m_SpeculativeContacts.insert(m_SpeculativeContacts.end(), boxContacts.begin(), boxContacts.end());
		}

		totalEnergy += it1->GetKineticEnergy();
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
	std::string initStrPath = FixBackslash(RelPathToAbs(SCRIPT_DIR ) + "/" + pyinitScript);
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
		vec2, // velocity
		vec2, // Position
		vec2, // Scale
		float, // Rotation about +z
        float, // mass
        float, // elasticity
		vec4, // Color
		std::string>;

	// Now loop through all named tuples in the script
	std::vector<EntInfo> v_EntInfo;
	check(pyinitModule.get_attr("r_Entities").convert(v_EntInfo), "Getting all Entity Info");

	// Preallocate all entities, since we'll need their pointers
	m_vEntities.resize(v_EntInfo.size());

	// Create individual entities and components
	for (int i = 0; i < m_vEntities.size(); i++) {
		// Get the tuple
		auto ei = v_EntInfo[i];

		// Unpack tuple
		vec2 vel(std::get<0>(ei));
		vec3 pos(std::get<1>(ei), 0.f);
		vec2 scale(std::get<2>(ei));
		float rot = std::get<3>(ei);
        float m = std::get<4>(ei);
        float e = clamp(std::get<5>(ei),0.f,1.f);
		vec4 color = glm::clamp(std::get<6>(ei), vec4(0), vec4(1));
		std::string pyEntModScript = std::get<7>(ei);

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
            AABB box(vel, vec2(pos), m, e, scale);
			box.SetEntity(&m_vEntities[i]);
			m_vAABB.push_back(box);
		}
		else if (colPrim == "OBB") {
			OBB box(vel, vec2(pos), m, e, scale, rot);
			box.SetEntity(&m_vEntities[i]);
			m_vOBB.push_back(box);
		}
		else {
			Circle circ(vel, vec2(pos), m, e, maxEl(scale));
			circ.SetEntity(&m_vEntities[i]);;
			m_vCircles.push_back(circ);
		}

		// Make drawable
		fquat rotQ(cos(rot / 2.f), vec3(0, 0, sin(rot / 2.f)));
		Drawable dr(iqmFile, color, quatvec(pos, rotQ), scale);
		dr.SetEntity(&m_vEntities[i]);
		m_vDrawables.push_back(dr);
	}

	// Fix entity pointers (I hate this)
	for (auto& circle : m_vCircles)
		circle.GetEntity()->SetColCmp(&circle);
	for (auto& box : m_vAABB)
		box.GetEntity()->SetColCmp(&box);
	for (auto& box : m_vOBB)
		box.GetEntity()->SetColCmp(&box);
	for (auto& drawable : m_vDrawables)
		drawable.GetEntity()->SetDrCmp(&drawable);
	
	// Expose in python, mapping ent ID to Exposed Entity
	// TODO Entities should be globally accessible via the PyLiaison module,
	// so find a way fo adding a container to it
	// PyDict_New...
	for (auto& ent : m_vEntities)
		ent.GetPyModule().call_function("AddEntity", ent.GetID(), &ent);
    
    // Contacts debug?
    pyinitModule.get_attr("CONTACT_DEBUG").convert(m_bShowContacts);
}