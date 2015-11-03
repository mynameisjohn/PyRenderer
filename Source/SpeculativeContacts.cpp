#include <glm/glm.hpp>

#include "SpeculativeContacts.h"
#include "RigidBody.h"
#include "Audible.h"
Contact::Contact(RigidBody_2D * a, RigidBody_2D * b, const vec2 p_a, const vec2 p_b, const vec2 nrm, const float d) :

pair{a, b},
pos{p_a, p_b},
//	A(a),
//    B(b),
//    pA(p_a),
//    pB(p_b),
	normal(nrm),
	dist(d),
//    curPenDist(0),
	isColliding(false)
{
    assert(a != b);
    
    float denom(0.f);
    for (int i=0; i<2; i++){
        rad[i] = perp(pos[i] - pair[i]->C);
        
        float rn = glm::dot(rad[i], normal);
        denom += 1.f / pair[i]->m;
        denom += powf(rn, 2) / pair[i]->GetInertia();
    }
    
    //assert(denom > kEPS);
    
    invMassI = 1.f / denom;
}

void Contact::ApplyImpulse(vec2 imp) {
    // 1/(m1+m2),  coef of restitution, epsilon
//    const float Msum_1 = 1.f / (A->m + B->m);
//    float mA = A->m + A->GetInertia() / powf(glm::dot(rA, rA), 1);
//    float mB = B->m + B->GetInertia() / powf(glm::dot(rB, rB), 1);
//    float vA = GetRelVelN_A();
//    float vB = GetRelVelN_B();
//    
//    const float Cr = 0.5f * (A->e + B->e);
//    const float Msum_1 = 1.f / (mA + mB);
//    
//    float pSum = mA * vA + mB * vB;
//    float Cr_diff = Cr * (vB - vA);
//    float n_vA = Msum_1 * (mB * Cr_diff + pSum);
//    float n_vB = Msum_1 * (-mA * Cr_diff + pSum);
//    
//    vec2 delV_A = (n_vA - vA) * normal;
//    vec2 delV_B = (n_vB - vB) * normal;
//    
//    // Change in angular velocity
//    float delW_A = glm::dot(delV_A, rA);
//    float delW_B = glm::dot(delV_B, rB);
//    
//    std::cout << rA << ", " << delV_A << std::endl;
//
//    // modify quantities
//    A->V += delV_A;
//    B->V += delV_B;
//
//    A->w += delW_A;
//    B->w += delW_B;
//
//    // Calculate new velocities
//    float pSum = A->m * vA + B->m * vB;
//    float Cr_diff = Cr * (vB-vA);
//    float n_vA = Msum_1*(B->m*Cr_diff + pSum);
//    float n_vB = Msum_1*(-A->m*Cr_diff + pSum);
//
//    // Velocity changes (could be done in terms of delP)
//    vec2 delV_A = (n_vA-vA) * c.normal;
//    vec2 delV_B = (n_vB-vB) * c.normal;
//
//    // Radius arms
//    vec2 rA = c.pos[0] - A->C;
//    vec2 rB = c.pos[1] - B->C;
//
//    // Change in angular velocity
//    float delW_A = A->m * (rA.x * delV_A.y - rA.y * delV_A.x) / A->GetInertia();
//    float delW_B = B->m * (rB.x * delV_B.y - rB.y * delV_B.x) / B->GetInertia();
//
//    // modify quantities
//    A->V += delV_A;
//    B->V += delV_B;
//
//    A->w += delW_A * 0.1f;
//    B->w += delW_B * 0.1f;
//
//    const float Cr = 0.5f * (A->e + B->e);
//

    for (int i=0; i<2; i++){
        const float sgn = i%2 ? -1.f : 1.f;
        pair[i]->V += sgn * imp / pair[i]->m;
        pair[i]->w += sgn * glm::dot(imp, rad[i]) / pair[i]->GetInertia();
    }
    
   // std::cout << A->GetKineticEnergy() + B->GetKineticEnergy() << std::endl;
}

inline vec2 relVel(const Contact * c, int i){
    return c->pair[i]->V + c->rad[i] * c->pair[i]->w;
}

vec2 Contact::GetRelVel_A() const {
    return relVel(this, 0);
}

vec2 Contact::GetRelVel_B() const{
    return relVel(this, 1);
}

float Contact::GetRelVelN_A() const {
    return glm::dot(GetRelVel_A(), normal);
}

float Contact::GetRelVelN_B() const{
    return glm::dot(GetRelVel_B(), normal);
}

// Get's the relative velocity of the two
// body contact system along contact normal
// (one contact's motion relative to the other)
//std::array<float, 2> Contact::relVel() const {
//    vec2 r0 = pos[0] - pair[0]->C;
//    vec2 V0 = pair[0]->V + perp(r0) * pair[0]->w;
//    
//    vec2 r1 = pos[1] - pair[1]->C;
//    vec2 V1 = pair[1]->V + perp(r1) * pair[1]->w;
//    
//    vec2 relV = V1 - V0;
//    
//    return {{glm::dot(V0, normal), glm::dot(V1, normal)}};
////	return pair[1]->V - pair[0]->V;
//}

uint32_t Solver::Solve(std::list<Contact>& contacts) {
    // Each iteration modifies a contacts curDV member,
    // which keeps track of the penetration distance between
    // contact points along their normal
    
    uint32_t numCollisions(0);
    
	for (int nIt = 0; nIt < m_nIterations; nIt++)
	{
        uint32_t numColIt(0);
		for (auto& c : contacts) {
            
//            if (nIt == 0)
//                std::cout << c.A << ", " << c.B << ", " << c.dist << std::endl;
            
//            float vA = c.GetRelVelN_A();
//            float vB = c.GetRelVelN_B();
        
            vec2 vA = c.GetRelVel_A();
            vec2 vB = c.GetRelVel_B();
            vec2 relV = vB - vA;
            float relNv = glm::dot(relV, c.normal);
           
            
            float remove = relNv + c.dist / globalTimeStep;
            
            if (remove < -kEPS){
                 //std::cout << relV << ", " << c.normal << "\n" << std::endl;
                
                float Cr_1 = 1.f + 0.5f * (c.pair[0]->e + c.pair[1]->e);
                float mag = remove * c.invMassI;
                
                vec2 n = c.normal;
                
                vec2 imp = n * (mag * Cr_1);
                                
                std::cout << imp << std::endl;
                
                c.ApplyImpulse(imp);
                
                numColIt++;
            }
        }
        
        // If there were any collisions this iteration,
        // add them to the total
        if (numColIt)
            numCollisions += numColIt;
        // Otherwise return, since we can't really do anything
        else
            break;
    }
    
    return numCollisions;
}
			// Find the relative velocity of the system along the normal
//			float relNv = glm::dot(c.normal, c.relVel());

			// Find the amount of velocity (along n) needed such that
			// adding dV * dT makes the objects touch (penetration dist)
//            std::array<float, 2> conVel = c.relVel();
//            float dV = (conVel[1] - conVel[0]) + c.dist / globalTimeStep;
//            
//            // Add it to the current penetration distance
//            c.curPenDist += dV;
            
//            if (nIt == 0)
//                std::cout << c.relVel() << ", " << dV << ", " << c.curPenDist << ", " << c.dist << std::endl;
            
            // If the penetration distance at this iteration is negative,
            // apply an impulse and hope it resolves the pentration
			//if (c.curPenDist < kEPS) {
//                vec2 r0 = c.pos[0] - c.pair[0]->C;
//                vec2 V0 = c.pair[0]->V + perp(r0) * c.pair[0]->w;
//                
//                vec2 r1 = c.pos[1] - c.pair[1]->C;
//                vec2 V1 = c.pair[1]->V + perp(r1) * c.pair[1]->w;
                
                // Calculate collision
                // A lot of this could be optimized, but right now
                // I'm just interested in clarity
//				RigidBody_2D * A = c.pair[0], *B = c.pair[1];
//
//				// 1/(m1+m2),  coef of restitution, epsilon
//				const float Msum_1 = 1.f / (A->m + B->m);
//				const float Cr = 0.5f * (A->e + B->e);
//                
//                // Find velocity components along collision normal
//                float vA = conVel[0];
//                float vB = conVel[1];
//                
//                // Calculate new velocities
//                float pSum = A->m * vA + B->m * vB;
//                float Cr_diff = Cr * (vB-vA);
//                float n_vA = Msum_1*(B->m*Cr_diff + pSum);
//                float n_vB = Msum_1*(-A->m*Cr_diff + pSum);
//                
//                // Velocity changes (could be done in terms of delP)
//                vec2 delV_A = (n_vA-vA) * c.normal;
//                vec2 delV_B = (n_vB-vB) * c.normal;
//                
//                // Radius arms
//                vec2 rA = c.pos[0] - A->C;
//                vec2 rB = c.pos[1] - B->C;
//                
//                // Change in angular velocity
//                float delW_A = A->m * (rA.x * delV_A.y - rA.y * delV_A.x) / A->GetInertia();
//                float delW_B = B->m * (rB.x * delV_B.y - rB.y * delV_B.x) / B->GetInertia();
//                
//                // modify quantities
//                A->V += delV_A;
//                B->V += delV_B;
//                
//                A->w += delW_A * 0.1f;
//                B->w += delW_B * 0.1f;
//                
//				// I don't like doing this
//				c.isColliding = true;
//			}
//            // Otherwise just reassign the pen dist to 0
//            else
//                c.curPenDist = 0.f;
            
//		}
//	}
//}