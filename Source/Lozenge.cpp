#include "RigidBody.h"
#include "SpeculativeContacts.h"

// Most of this isn't implemented, since I don't know what utility
// Lozenges really give me over OBB (besides being rounded at edges)

Lozenge::Lozenge() :
	RigidBody_2D(),
	r(0.5f),
	R(0.f)
{}

Lozenge::Lozenge(vec2 vel, vec2 c, float mass, float elasticity, vec2 s, float th) :
	RigidBody_2D(V, C, m, e, th) {
	float diameter = minEl(s);
	r = Lozenge::ALPHA * diameter / 2.f;
	R = (1.f - Lozenge::ALPHA) * (s - vec2(diameter));
}

// Between Lozenges
std::list<Contact> Lozenge::GetClosestPoints(const Lozenge& other) const {
	// NYI
	std::list<Contact> ret;
	return ret;
}

// Between a Lozenge and an AABB
std::list<Contact> Lozenge::GetClosestPoints(const AABB& other) const {
	// NYI
	std::list<Contact> ret;
	return ret;
}

// Between a Lozenge and a Capsule
std::list<Contact> Lozenge::GetClosestPoints(const Capsule& other) const {
	// NYI
	std::list<Contact> ret;
	return ret;
}

// Between a Lozenge and a Circle
std::list<Contact> Lozenge::GetClosestPoints(const Circle& other) const {
	// NYI
	std::list<Contact> ret;
	return ret;
}

std::list<Contact> Lozenge::GetClosestPoints(const OBB& other) const {
	// NYI
	std::list<Contact> ret;
	return ret;
}

bool Lozenge::IsOverlapping(const Circle& other) const {
	// NYI
	return false;
}

// Between Lozenge and OBB (see OBB impl)
bool Lozenge::IsOverlapping(const OBB& other) const {
	// NYI
	return false;
}

bool Lozenge::IsOverlapping(const Lozenge& other) const {
	// NYI
	return false;
}

bool Lozenge::IsOverlapping(const AABB& other) const {
	// NYI
	return false;
}

bool Lozenge::IsOverlapping(const Capsule& other) const {
	// NYI
	return false;
}

bool Lozenge::IsOverlapping(const Lozenge& other) const {
	// NYI
	return false;
}