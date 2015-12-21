#include "RigidBody.h"
#include "SpeculativeContacts.h"

AABB::AABB() :
	RigidBody_2D(),
	R(1)
{}

AABB::AABB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r) :
	RigidBody_2D(vel, c, mass, elasticity, 0.f),
	R(r)
{}

AABB::AABB(vec2 v, float mass, float el, float x, float y, float w, float h) :
	RigidBody_2D(v, vec2(x, y) + vec2(w, h) / 2.f, mass, el, 0.f),
	R(vec2(w, h) / 2.f)
{
	R = vec2(w, h) / 2.f;
}

// This was written to prevent AABB rotation,
// but something better must arise
void AABB::ChangeVel(vec2 newV, vec2 rad) {
	V = newV;
}

// I = (m/3) * (R.x^2 + R.y^2)
float AABB::GetInertia() const {
	return (m / 3.f) * (pow(R.x, 2) + pow(R.y, 2));
}

// TODO write these for OBB
float AABB::width()const { return 2.f*R.x; }
float AABB::height()const { return 2.f*R.y; }
float AABB::left()const { return C.x - R.x; }
float AABB::right()const { return C.x + R.x; }
float AABB::top()const { return C.y + R.y; }
float AABB::bottom()const { return C.y - R.y; }

// Overlap of two AABBs
bool AABB::IsOverlapping(const AABB& other) const {
	return
		(left() > other.right() || right() < other.left()) &&
		(bottom() > other.top() || top() < other.bottom());
}

// Overlap of circle and AABB
bool AABB::IsOverlapping(const Circle& other) const {
	return other.IsOverlapping(*this);
}

// Overlap of AABB - OBB
bool AABB::IsOverlapping(const OBB& other) const {
	return other.IsOverlapping(*this);
}

bool AABB::IsOverlapping(const Capsule& other) const {
	return other.IsOverlapping(*this);
}

bool AABB::IsOverlapping(const Lozenge& other) const {
	return other.IsOverlapping(*this);
}

// Clamp a point within the AABBs extent
vec2 AABB::clamp(vec2 p) const {
	return  glm::clamp(p, C - R, C + R);
}

// I had to write this special to zero out angular stuff
void AABB::Integrate() {
	th = 0.f;
	w = 0.f;
	RigidBody_2D::Integrate();
}

// Closest points between AABB and Circle (see circle impl)
std::list<Contact> AABB::GetClosestPoints(const Circle& other) const {
	return other.GetClosestPoints(*this);
}

// AABB-OBB, see OBB impl
std::list<Contact> AABB::GetClosestPoints(const OBB& other) const {
	return other.GetClosestPoints(*this);
}

// Closest points between two AABBs
std::list<Contact> AABB::GetClosestPoints(const AABB& other) const {
	// watch for low values here
	vec2 d = C - other.C;
	vec2 n = glm::normalize(d);
	vec2 a_pos = clamp(d);
	vec2 b_pos = other.clamp(d);
	float dist = glm::length(a_pos - b_pos);
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

// Closest points between two AABBs
std::list<Contact> AABB::GetClosestPoints(const Lozenge& other) const {
	return other.GetClosestPoints(*this);
}

// Closest points between two AABBs
std::list<Contact> AABB::GetClosestPoints(const Lozenge& other) const {
	return other.GetClosestPoints(*this);
}


// Probably a cuter way of writing this
vec2 AABB::GetFaceNormalFromPoint(vec2 p) const {
	vec2 n;

	if (p.x < right() && p.x > left()) {
		if (p.y < bottom())
			n = vec2(0, -1);
		else
			n = vec2(0, 1);
	}
	else {
		if (p.x < left())
			n = vec2(-1, 0);
		else
			n = vec2(1, 0);
	}

	return n;
}

// Get verts, clockwise
vec2 AABB::GetVert(uint32_t idx) const {
	vec2 ret(0);
	switch (idx % 4) {
	case 0:
		return C + R;
	case 1:
		return C + vec2(R.x, -R.y);
	case 2:
		return C - R;
	case 3:
	default:
		return C + vec2(-R.x, R.y);
	}
}

// Get normal, clockwise
vec2 AABB::GetNormal(uint32_t idx) const {
	switch (idx % 4) {
	case 0:
		return vec2(1, 0);
	case 1:
		return vec2(0, -1);
	case 2:
		return vec2(-1, 0);
	case 3:
	default:
		return vec2(0, 1);
	}
}