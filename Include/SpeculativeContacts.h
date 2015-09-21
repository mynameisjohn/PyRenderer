#pragma once

class RigidBody_2D;

#include "Util.h"
#include <vec2.hpp>
#include <list>

// Speculative contacts stuff
struct Contact {
	RigidBody_2D * pair[2];
	vec2 pos[2];
	vec2 normal;
	float impulse;
	float dist;
	// He owns the objects themselves, which I could probably get away with too
	Contact(RigidBody_2D * a, 
		RigidBody_2D * b, 
		const vec2 p_a,
		const vec2 p_b,
		const vec2 nrm, 
		const float i, 
		const float d);
	void ApplyImpulse(float mag);
	vec2 relVel() const;
};

// Really just a namespace...
class Solver {
public:
	Solver() {}
	void operator()(std::list<Contact>& contacts);
};