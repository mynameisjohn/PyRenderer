#include <glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e, Entity * pEnt) :
	V(V),
	C(C),
	m(m),
	e(e),
	lastT(0),
	PyComponent(pEnt)
{}

void RigidBody_2D::ApplyCollisionImpulse(RigidBody_2D * other) {
	// This is expensive, but right now I really don't care
	if (glm::length(V - other->V) < 0.001f)
		return;

	float Msum_1 = 1.f / (m + other->m); // denominator
	float Cr = 0.5f * (e + other->e); // coef of rest
	vec2 P1 = m * V, P2 = other->m * other->V;
	vec2 A = P1 + P2, B = Cr * (P1 - P2);
	vec2 v1 = (A - B) * Msum_1;
	vec2 v2 = (A + B) * Msum_1;
	vec2 n = glm::normalize(C - other->C);
	V = v1*n;
	other->V = -v2*n;

	// Not the time
	//m_pEntity->GetPyModule().call_function("HandleCollision", m_pEntity->GetID(),
	//	other->m_pEntity->GetID());
}

// I guess this just advances the object?
void RigidBody_2D::Integrate() {
	const float dT = 0.005f; // TODO better integration methods?
	vec2 delta = dT * V;
	lastT += delta; // Entity should reset this
	C += delta;
}

// TODO get this to post a translation message
bool RigidBody_2D::PyUpdate() {
	// For now just hack it
	m_pEntity->GetPyModule().call_function("UpdateCollision", m_pEntity->GetID());
	return true;
}

bool RigidBody_2D::PyExpose(const std::string& name, PyObject * module) {
	Python::Expose_Object(this, name, module);

	return true;
}

bool Circle::IsOverlapping(const Circle& other) const {
	float dist = glm::length(C - other.C);
	float totalRadius = r + other.r;

	return (dist < totalRadius);
}

bool Circle::IsOverlapping(const AABB& other) const {
	bool x = (other.left() > C.x + r) || (other.right() > C.y - r);
	bool y = (other.bottom() > C.x + r) || (other.top() > C.y - r);
	return x && y;
}

Circle::Circle(vec2 V, vec2 C, float m, float e, float radius, Entity * pEnt) :
	r(radius),
	RigidBody_2D(V, C, m, e, pEnt)
{}

AABB::AABB(float x, float y, float w, float h, Entity * pEnt):
	R(vec2(w, h) / 2.f),
	RigidBody_2D(vec2(0),vec2(x,y),1.f,1.f, pEnt)
{}

float AABB::width()const { return 2.f*R.x; }
float AABB::height()const { return 2.f*R.y; }
float AABB::left()const { return C.x - R.x; }
float AABB::right()const { return C.x + R.x; }
float AABB::top()const { return C.y + R.y; }
float AABB::bottom()const { return C.y - R.y; }

bool AABB::IsOverlapping(const AABB& other) const {
	return
		(left() > other.right() || right() < other.left()) &&
		(bottom() > other.top() || top() < other.bottom());
}

bool AABB::IsOverlapping(const Circle& other) const {
	return other.IsOverlapping(*this);
}

vec2 AABB::clamp(vec2 p) const {
	return  glm::clamp(p, -R, R);
}

// Closest Point methods

// Kind of a waste
std::list<Contact> Circle::GetClosestPoints(const Circle& other) const {
	// watch for low values here
	// find and normalize distance
	vec2 d = other.C - C;
	vec2 n = glm::normalize(d);
	// contact points along circumference
	vec2 a_pos = C + n*r;
	vec2 b_pos = other.C - n*other.r;
	// distance between circumferences
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, 0.f, dist);
	return{ c };
}

std::list<Contact> Circle::GetClosestPoints(const AABB& other) const {
	// watch for low values here
	vec2 d = other.C - C;
	vec2 n = glm::normalize(d);
	vec2 a_pos = C + n*r;
	// d points from the circle to the box, so negate and clamp
	vec2 b_pos = other.clamp(-d);
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, 0.f, dist);
	return{ c };
}

std::list<Contact> AABB::GetClosestPoints(const Circle& other) const {
	return other.GetClosestPoints(*this);
}

std::list<Contact> AABB::GetClosestPoints(const AABB& other) const {
	// watch for low values here
	vec2 d = C - other.C;
	vec2 n = glm::normalize(d);
	vec2 a_pos = clamp(d);
	vec2 b_pos = other.clamp(-d);
	float dist = glm::length(a_pos-b_pos);
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, a_pos, b_pos, n, 0.f, dist);
	return{ c };
}