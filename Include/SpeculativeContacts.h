#pragma once

class RigidBody_2D;

#include "Util.h"
#include <glm/vec2.hpp>
#include <list>

// Speculative contacts stuff
struct Contact {
    // Pointers to the two objects that may collide
    union{
        RigidBody_2D * pair[2];
        struct {RigidBody_2D * A, * B;};
    };
    // The positions of the contact points
    union{
        vec2 pos[2];
        struct{ vec2 pA; vec2 pB; };
    };
    // Radius arms
    union{
        vec2 rad[2];
        struct{vec2 rA; vec2 rB;};
    };
    // Collision normal
	vec2 normal;
    // Distance between contacts
	float dist;
    // Inverse impulse mass denominator
    float invMassI;
    
    //float curPenDist;
    // Really just used to see if things are currently colliding, I can do better
	bool isColliding;
    
    // Constructor
	Contact(RigidBody_2D * a,
		RigidBody_2D * b,
		const vec2 p_a,
		const vec2 p_b,
		const vec2 nrm, 
		const float d);
    
    // Apply some collision impulse
	void ApplyImpulse(vec2 imp);
    
    // Get the relative velocity of A and B
    vec2 GetRelVel_A() const;
    vec2 GetRelVel_B() const;
    
    // Same, along normal
    float GetRelVelN_A() const;
    float GetRelVelN_B() const;
    
    //std::array<float, 2> relVel() const;
//	float relVel() const;
};

// Really just a namespace...
class Solver {
	uint32_t m_nIterations;
public:
	Solver(int nIt = 10) : m_nIterations(nIt) {}
    
    uint32_t Solve(std::list<Contact>& contacts);
	//void operator()(std::list<Contact>& contacts);
};