#include <glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e) :
	V(V),
	C(C),
	m(m),
	e(e),
	lastT(0)
{}

bool Circle::PyExpose(const std::string& name, PyObject * module) {
	Python::Expose_Object(this, name, module);

	return true;
}

bool Circle::IsColliding(Circle& other) {
	float dist = glm::length(C - other.C);
	float totalRadius = r + other.r;

	return (dist < totalRadius);
}

void Circle::ApplyCollision(Circle& other) {
	// This is expensive, but right now I really don't care
	if (glm::length(V - other.V) < 0.001f)
		return;

	float Msum_1 = 1.f / (m + other.m); // denominator
	float Cr = 0.5f * (e + other.e); // coef of rest
	vec2 P1 = m * V, P2 = other.m * other.V;
	vec2 A = P1 + P2, B = Cr * (P1 - P2);
	vec2 v1 = (A - B) * Msum_1;
	vec2 v2 = (A + B) * Msum_1;
	vec2 n = glm::normalize(C - other.C);
	V = v1*n;
	other.V = -v2*n;
	m_pEntity->GetPyModule().call_function("HandleCollision", m_pEntity->GetID(),
		other.m_pEntity->GetID());
}

// I guess this just advances the object?
void Circle::Update() {
	const float dT = 0.005f; // TODO better integration methods?
	vec2 delta = dT * V;
	lastT += delta; // Entity should reset this
	C += delta;
}

// TODO get this to post a translation message
bool Circle::PyUpdate() {
	// For now just hack it
	m_pEntity->GetPyModule().call_function("UpdateCollision", m_pEntity->GetID());
	return true;
}

Circle::Circle(vec2 V, vec2 C, float m, float e, float r, Entity * pEnt) :
	RigidBody_2D(V, C, m, e), PyComponent(pEnt)
{
	this->r = r;
}

// Kind of a waste
std::list<Contact> Circle::GetClosestPoints(const Circle& other) {
	// watch for low values here
	vec2 d = C - other.C;
	vec2 n = glm::normalize(d);
	vec2 a_pos = C + n*r;
	vec2 b_pos = other.C + n*other.r;
	float dist = glm::length(d) - (r + other.r);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, 0.f, dist);
	return{ c };
}