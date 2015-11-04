#pragma once

class RigidBody_2D;

#include "Util.h"
#include <glm/vec2.hpp>
#include <list>

// Speculative contacts stuff
struct Contact {
    union{ // Pointers to the two objects that may collide
        RigidBody_2D * pair[2];
        struct {RigidBody_2D * A, * B;};
    };
    union{ // The positions of the contact points
        vec2 pos[2];
        struct{ vec2 pA; vec2 pB; };
    };
    union{ // Radius arms
        vec2 rad[2];
        struct{vec2 rA; vec2 rB;};
    };
    
	vec2 normal; // Collision normal
	float dist; // Distance between contacts
    float invMassI; // Inverse impulse mass denominator
	float curImpulse; // Accumulated impulse
	bool isColliding; // Really just used to see if things are currently colliding, I can do better
    
    // Constructor
	Contact(RigidBody_2D * a,
		RigidBody_2D * b,
		const vec2 p_a,
		const vec2 p_b,
		const vec2 nrm, 
		const float d);
    
    // Apply some collision impulse
	void ApplyImpulse(float mag);
    
    // Get the relative velocity of A and B
    vec2 GetVel_A() const;
    vec2 GetVel_B() const;
    
    // Same, along normal
    float GetVelN_A() const;
    float GetVelN_B() const;
};

// Really just a namespace...
class Solver {
	uint32_t m_nIterations;
public:
	Solver(int nIt) : m_nIterations(nIt) {}
    
    uint32_t Solve(std::list<Contact>& contacts);
	uint32_t operator()(std::list<Contact>& contacts);
};