#include "RigidBody.h"
#include "SpeculativeContacts.h"

// Should be moment of inertia....
float Circle::GetInertia() const {
	return 0.5f * m * pow(r, 2);
}

Circle::Circle() :
	RigidBody_2D(),
	r(1)
{}

Circle::Circle(vec2 V, vec2 C, float m, float e, float radius, float th) :
	RigidBody_2D(V, C, m, e, th),
	r(radius)
{}

// Between circles
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

// Between a circle and an AABB
std::list<Contact> Circle::GetClosestPoints(const AABB& other) const {
	// d points from the circle to the box, so negate and clamp
	vec2 b_pos = other.clamp(C);
	vec2 n = glm::normalize(b_pos - C);

	vec2 a_pos = C + r*n;
	float dist = glm::length(a_pos - b_pos);

	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

// Between a circle and a Capsule
std::list<Contact> Circle::GetClosestPoints(const Capsule& other) const {
	return other.GetClosestPoints(*this);
}

// Between a circle and a Lozenge
std::list<Contact> Circle::GetClosestPoints(const Lozenge& other) const {
	return other.GetClosestPoints(*this);
}

// Also see OBB impl
std::list<Contact> Circle::GetClosestPoints(const OBB& other) const {
	return other.GetClosestPoints(*this);
}

// Between Circle and OBB (see OBB impl)
bool Circle::IsOverlapping(const OBB& other) const {
	return other.IsOverlapping(*this);
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

bool Circle::IsOverlapping(const Capsule& other) const {
	return other.IsOverlapping(*this);
}

bool Circle::IsOverlapping(const Lozenge& other) const {
	return other.IsOverlapping(*this);
}