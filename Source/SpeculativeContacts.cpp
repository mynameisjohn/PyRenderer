#include <glm/glm.hpp>

#include "SpeculativeContacts.h"
#include "RigidBody.h"
#include "Audible.h"
Contact::Contact(RigidBody_2D * a, RigidBody_2D * b, const vec2 p_a, const vec2 p_b, const vec2 nrm, const float d) :
	pair{ a, b },
	pos{ p_a,p_b },
	normal(nrm),
	dist(d),
    curPenDist(0),
	isColliding(false)
{}

void Contact::ApplyImpulse(float mag) {
	vec2 I = normal*mag;
	pair[0]->V += I/pair[0]->m;
	pair[1]->V -= I/pair[1]->m;
}

vec2 Contact::relVel() const {
	return pair[1]->V - pair[0]->V;
}

void Solver::operator()(std::list<Contact>& contacts) {
    // Each iteration modifies a contacts curDV member,
    // which keeps track of the penetration distance between
    // contact points along their normal
	for (int nIt = 0; nIt < m_nIterations; nIt++)
	{
		for (auto& c : contacts) {
			// Find the relative velocity of the system along the normal
			float relNv = glm::dot(c.normal, c.relVel());

			// Find the amount of velocity (along n) needed such that
			// adding dV * dT makes the objects touch (penetration dist)
			float dV = relNv + c.dist / globalTimeStep;
            
            // Add it to the current penetration distance
            c.curPenDist += dV;
            
            //std::cout << relNv << ", " << dV << ", " << c.curPenDist << std::endl;
            
            // If the penetration distance at this iteration is negative,
            // apply an impulse and hope it resolves the pentration
			if (c.curPenDist < 0.f) {
                // Calculate collision
                // A lot of this could be optimized, but right now
                // I'm just interested in clarity
				RigidBody_2D * A = c.pair[0], *B = c.pair[1];

				// 1/(m1+m2),  coef of restitution, epsilon
				const float Msum_1 = 1.f / (A->m + B->m);
				const float Cr = 0.5f * (A->e + B->e);
                
                // Find velocity components along collision normal
                float vA = glm::dot(A->V, c.normal);
                float vB = glm::dot(B->V, c.normal);
                
                // Calculate new velocities
                float pSum = A->m * vA + B->m * vB;
                float Cr_diff = Cr * (vB-vA);
                float n_vA = Msum_1*(B->m*Cr_diff + pSum);
                float n_vB = Msum_1*(-A->m*Cr_diff + pSum);
                
                // Velocity changes (could be done in terms of delP)
                vec2 delV_A = (n_vA-vA) * c.normal;
                vec2 delV_B = (n_vB-vB) * c.normal;
                
                // Radius arms
                vec2 rA = c.pos[0] - A->C;
                vec2 rB = c.pos[1] - B->C;
                
                // Change in angular velocity
                float delW_A = A->m * (rA.x * delV_A.y - rA.y * delV_A.x) / A->GetInertia();
                float delW_B = B->m * (rB.x * delV_B.y - rB.y * delV_B.x) / B->GetInertia();
                
                // modify quantities
                A->V += delV_A;
                B->V += delV_B;
                
                A->w += delW_A;
                B->w += delW_B;

				// I don't like doing this
				c.isColliding = true;
			}
            // Otherwise just reassign the pen dist to 0
            else
                c.curPenDist = 0.f;
            
		}
	}
}