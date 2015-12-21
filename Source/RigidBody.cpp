#include <glm/glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

////////////////////////////////////////////////////////////////////////////////////////
// RigidBody_2D functions

RigidBody_2D::RigidBody_2D() :
	V(0),
	C(0),
	m(1),
	e(1),
	th(0),
	w(0)
{}

RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e, float th) :
	V(V),
	C(C),
	m(m),
	e(e),
	th(th),
	w(0)
{}

// I guess this just advances the object?
void RigidBody_2D::Integrate() {
//	const float dT = 0.005f; // TODO better integration methods?
	vec2 delta = globalTimeStep * V;
	C += delta;
    
    //  Decay anglar vel for now
	th += w * globalTimeStep;
    //w *= 0.99f;

	// I used to have python do this, but what's the point?
	m_pEntity->SendMessage(int(Entity::CompID::DRAWABLE),
		int(Entity::MsgID::DR_TRANSLATE));
}

// Return some basic physical quantities,
// I'm fairly certain noone is calling these
vec2 RigidBody_2D::GetMomentum() const {
	return m * V;
}

float RigidBody_2D::GetKineticEnergy() const {
	return 0.5f * m * glm::dot(V, V);
}

// Return the graphical quatvec transform of a rigid body
quatvec RigidBody_2D::GetQuatVec() const {
    vec3 T(C, 0.f);
    fquat Q(cos(th/2.f), vec3(0.f, 0.f, sin(th/2.f)));
    return quatvec(T, Q);
}

// TODO all of this is no being done in SpeculativeContacts.cpp
void RigidBody_2D::ChangeVel(vec2 newV, vec2 rad) {
	vec2 delV = newV - V;
	V = newV;
	vec2 perp(-rad.y, rad.x);
	w += glm::dot(delV, perp);
}

////////////////////////////////////////////////////////////////////////////////////////
// AABB functions



// FeaturePair stuff, not sure where this belongs
FeaturePair::FeaturePair(float d, float cd, int f, int v, Type t) :
dist(d),
c_dist(cd),
fIdx(f),
vIdx(v),
T(t)
{}