#include <glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

RigidBody_2D::RigidBody_2D():
	V(0),
	C(0),
	m(1),
	e(1)
{}


RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e) :
	V(V),
	C(C),
	m(m),
	e(e)
{}

// I guess this just advances the object?
void RigidBody_2D::Integrate() {
	const float dT = 0.005f; // TODO better integration methods?
	vec2 delta = dT * V;
	C += delta;

	// I used to have python do this, but what's the point?
	m_pEntity->PostMessage(int(Entity::CompID::DRAWABLE),
		int(Entity::MsgID::DR_TRANSLATE));
}

vec2 RigidBody_2D::GetMomentum() const {
	return m * V;
}

float RigidBody_2D::GetKineticEnergy() const {
	return 0.5f * m * glm::dot(V, V);
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

Circle::Circle() :
	r(1),
	RigidBody_2D()
{}

Circle::Circle(vec2 V, vec2 C, float m, float e, float radius) :
	r(radius),
	RigidBody_2D(V, C, m, e)
{}

AABB::AABB() :
	R(1),
	RigidBody_2D()
{}

AABB::AABB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r):
	R(r),
	RigidBody_2D(vel, c, mass, elasticity)
{}

AABB::AABB(vec2 v, float mass, float el, float x, float y, float w, float h):
	R(vec2(w, h) / 2.f),
	RigidBody_2D(v,vec2(x,y)+ vec2(w, h) / 2.f,mass, el)
{
	R = vec2(w, h) / 2.f;
}

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
	return  glm::clamp(p, C-R, C+R);
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
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

std::list<Contact> Circle::GetClosestPoints(const AABB& other) const {
	// watch for low values here
	//vec2 d = other.C - C;
	//vec2 n = glm::normalize(d);
	//vec2 a_pos = C + n*r;

	// d points from the circle to the box, so negate and clamp
	vec2 b_pos = other.clamp(C);
	vec2 n = glm::normalize(b_pos - C);


	vec2 a_pos = C + r*n;
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
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
	vec2 b_pos = other.clamp(d);
	float dist = glm::length(a_pos-b_pos);
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

// Impuse application methods
static void ApplyCollisionImpulse_Generic(RigidBody_2D * a, RigidBody_2D * b, vec2 N) {
	// Solve 1-D collision along normal between objects
	glm::vec2 n = glm::normalize(a->C - b->C);
	float v1i = glm::dot(n, a->V);
	float v2i = glm::dot(n, b->V);

	// 1/(m1+m2),  coef of restitution
	const float Msum_1 = 1.f / (a->m + b->m);
	const float Cr = 0.5f * (a->e + b->e);

	// find momentum, solve for final velocity
	float pa = a->m * v1i, pb = b->m * v2i;
	float Cr_diff = Cr*(v2i - v1i);
	float psum = pa + pb;

	float v1f = Msum_1*(b->m*Cr_diff + psum);
	float v2f = Msum_1*(-a->m*Cr_diff + psum);

	// do this more cute (needed for "conservation of momentum")
	if (fabs(v1f) < 0.001f) v1f = 0.f;
	if (fabs(v2f) < 0.001f) v2f = 0.f;

	std::cout << "p0: " << pa + pb << ", p1: " << v1f*a->m + v2f*b->m << std::endl;
	
	// apply velocity along normal direction
	a->V = v1f*n;
	b->V = v2f*n;
}

void RigidBody_2D::ApplyCollisionImpulse(RigidBody_2D * const other, vec2 n) {
	ApplyCollisionImpulse_Generic(this, other, n);
}