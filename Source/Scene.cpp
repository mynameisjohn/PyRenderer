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

	GLuint pmvHandle = m_Shader["u_PMV"];
	GLuint clrHandle = m_Shader["u_Color"];
	mat4 P = m_Camera.GetMat();
	auto drawIt = [pmvHandle, clrHandle, &P](Drawable& dr, vec4 c) {
		mat4 PMV = P * dr.GetMV();
		glUniformMatrix4fv(pmvHandle, 1, GL_FALSE, glm::value_ptr(PMV));
		glUniform4f(clrHandle, c[0], c[1], c[2], c[3]);
		dr.Draw();
	};


	
	for (auto& dr : m_vDrawables) {
		mat4 PMV = m_Camera.GetMat() * dr.GetMV();
		vec4 c = dr.GetColor();
		drawIt(dr, c);
	}

	// Until I can think of a better way to do this
	for (auto& cap : m_vCapsules) {
		auto endPoints = cap.GetEndpoints();
		Circle left(vec2(0), endPoints[0], 0, 0, cap.r);
		Circle right(vec2(0), endPoints[1], 0, 0, cap.r);
		OBB rect(vec2(0), cap.C, 0, 0, vec2(cap.L, 2.f * cap.r), cap.th);
		vec4 color(1);

		Drawable circ("circle.iqm", vec4(1), left.GetQuatVec(), vec2(left.r));
		drawIt(circ, color);
		
		circ.Translate(vec3(endPoints[1] - endPoints[0], 0.f));
		drawIt(circ, color);

		Drawable quad("quad.iqm", vec4(1), rect.GetQuatVec(), rect.R);
		drawIt(quad, color);
	}

    bool contactDebug(false);
    m_MainPyModule.get_attr("CONTACT_DEBUG").convert(contactDebug);
    if (contactDebug){
        for (auto& contact : m_SpeculativeContacts) {
            for (auto& p : contact.pos) {
                fquat rot(1, 0, 0, 0);
                
                //std::cout << contact.normal << ", " << 0.5f * (contact.normal + vec2(1)) << std::endl;
                
                // Contact color is normal remapped to 0,1
                const vec2 v1(1);
                float rndBlue = 1.f;//uint32_t(rand()) / float(RAND_MAX);
                vec4 ctColor = vec4(remap(contact.normal, -v1, v1, vec2(), v1), rndBlue, 1.f);
                quatvec ctTr = quatvec(vec3(p, -1.f), rot);
                vec2 ctScl = vec2(0.2f);
                
                Drawable drContact("circle.iqm", ctColor, ctTr, ctScl);
				drawIt(drContact, ctColor);
            }
        }
    }

	return int(m_vDrawables.size());
}

int Scene::Update() {
	int nCols(0);
	float totalEnergy(0.f);
	m_SpeculativeContacts.clear();

    // Integrate objects
	for (auto& c : m_vCircles)
		c.Integrate();

	for (auto& b : m_vAABB)
		b.Integrate();

	for (auto& b : m_vOBB)
		b.Integrate();

	// Collisions - For every circle
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

    // Let python ents know about collision
	// I don't like using the bool here, so figure something else out
	m_ContactSolver.Solve(m_SpeculativeContacts);
	for (auto& c : m_SpeculativeContacts) {
		if (c.isColliding)
        {
			Entity * e1 = c.pair[0]->GetEntity();
			Entity * e2 = c.pair[1]->GetEntity();
			if (e1 && e2) {
                m_MainPyModule.call_function("HandleCollision", e1->GetID(), e2->GetID());
//				e1->GetPyModule().call_function("HandleCollision", e1->GetID(), e2->GetID());
//				e2->GetPyModule().call_function("HandleCollision", e2->GetID(), e1->GetID());
			}
		}
	}
    
    // Update python (wh
    m_MainPyModule.call_function("Update");

    // Handle all entity messages (Update is a bad name)
	for (auto& e : m_vEntities)
		e.Update();
    
    m_EffectManager.Update();

	return nCols;
}

Python::Object Scene::GetPyModule() const{
    return m_MainPyModule;
}