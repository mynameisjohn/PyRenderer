#pragma once

class RigidBody_2D;

#include "Util.h"
#include <glm/vec2.hpp>
#include <list>

// Speculative contacts stuff
struct Contact {
	RigidBody_2D * pair[2];
	vec2 pos[2];
	vec2 normal;
	float dist;
	bool isColliding;
	// He owns the objects themselves, which I could probably get away with too
	Contact(RigidBody_2D * a,
		RigidBody_2D * b,
		const vec2 p_a,
		const vec2 p_b,
		const vec2 nrm, 
		const float d);
	void ApplyImpulse(float mag);
	vec2 relVel() const;
};

// Really just a namespace...
class Solver {
	uint32_t m_nIterations;
public:
	Solver(int nIt = 10) : m_nIterations(nIt) {}
	void operator()(std::list<Contact>& contacts);
};