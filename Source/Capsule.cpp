#include "RigidBody.h"
#include "SpeculativeContacts.h"

#include <glm/mat2x2.hpp>

Capsule::Capsule() :
	RigidBody_2D(),
	r(0.5f),
	L(0.f)
{}

Capsule::Capsule(vec2 vel, vec2 c, float mass, float elasticity, vec2 s, float th) :
	RigidBody_2D(vel, c, mass, elasticity, th),
	r(s.y),
	L(fabs(s.x-s.y))
{}

bool Capsule::IsOverlapping(const Circle& other) const {
	// Get distance to circle from segment
	float dist;

	return (dist < r);
}

bool Capsule::IsOverlapping(const AABB& other) const {
	// Get distance to box from segment
	float dist;

	return (dist < r);
}

bool Capsule::IsOverlapping(const OBB& other) const {
	// Get distance to box from segment
	float dist;

	return (dist < r);
}

bool Capsule::IsOverlapping(const Capsule& other) const {
	// Get distance to box from segment
	float dist;

	return (dist < r);
}

bool Capsule::IsOverlapping(const Lozenge& other) const {
	// Get distance to box from segment
	float dist;

	return (dist < r);
}

std::list<Contact> Capsule::GetClosestPoints(const Circle& other) const {
	// Get closest point on circle
	vec2 d = C - other.C;
	float dist = glm::length(d);
	vec2 n = d / n;
	vec2 circPos = other.C + other.r * n;

	// Project circPos onto segment

	// Same thing
	vec2 projectedPoint;
	vec2 capPos = projectedPoint + r * n;
	
	// Make contact
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, circPos, capPos, n, dist);

	return{ c };
}

std::list<Contact> Capsule::GetClosestPoints(const AABB& other) const {
	auto endPoints = GetEndpoints();

	// Loop through all edges of AABB
	// project edge onto segment
	// smallest distance makes contact
	float minDist(FLT_MAX);
	uint32_t minIdx(4);
	vec2 boxPos;
	for (uint32_t i = 0; i < 4; i++) {
		vec2 e0 = other.GetVert(i);
		vec2 e1 = other.GetVert(i + 1);

		// Find shortest dist between
		// e0==e1 and pA==pB
		float dist;
		if (dist < minDist) {
			minIdx = i;
			// assign boxPos
		}
	}

	// project boxPos onto segment
	vec2 projectedPoint;
	vec2 d = boxPos - projectedPoint;
	float dist = glm::length(d);
	vec2 n = d / dist;
	vec2 capPos = projectedPoint + r * n;

	// Make contact
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, boxPos, capPos, n, dist);

	return{ c };
}

std::array<vec2, 2> Capsule::GetEndpoints() const {
	// C + or - L/2 along segment dir
	vec2 ofs = getRotMat() * vec2(L / 2, 0.f);
	vec2 pA = C - ofs;
	vec2 pB = C + ofs;

	return{ pA, pB };
}

// Calls OBB(this), which calls AABB(this), which calls above (big stack, who cares)
std::list<Contact> Capsule::GetClosestPoints(const OBB& other) const {
	return ((AABB&)other).GetClosestPoints(*this);
}

std::list<Contact>Capsule::GetClosestPoints(const Capsule& other) const {
	// Project our segment onto their segment
	auto ourEndpoints = GetEndpoints();
	auto theirEndPoints = other.GetEndpoints();

	vec2 usProjected;
	vec2 themProjected;

	vec2 d = themProjected - usProjected;
	float dist = glm::length(d);
	vec2 n = d / n;

	vec2 ourPos = usProjected + n * r;
	vec2 theirPos = themProjected + n * other.r;

	// Make contact
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, ourPos, theirPos, n, dist);

	return{ c };
}

std::list<Contact> Capsule::GetClosestPoints(const Lozenge& other) const {
	return other.GetClosestPoints(*this);
}
