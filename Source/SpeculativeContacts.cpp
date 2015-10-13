#include <glm/glm.hpp>

#include "SpeculativeContacts.h"
#include "RigidBody.h"
#include "Audible.h"
Contact::Contact(RigidBody_2D * a, RigidBody_2D * b, const vec2 p_a, const vec2 p_b, const vec2 nrm, const float d) :
	pair{ a, b },
	pos{ p_a,p_b },
	normal(nrm),
	dist(d),
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
	const float dT(0.005f);
	
	for (int nIt = 0; nIt < m_nIterations; nIt++)
	{
		for (auto& c : contacts) {
			// Find the relative velocity of the system along the normal
			float relNv = glm::dot(c.normal, c.relVel());

			// Find the amount of velocity to remove, along the normal, such that the objects touch

			// Find the amount of velocity (along n) needed such that
			// adding dV * dT makes the objects touch
			float dV = relNv + c.dist / globalTimeStep;

			if (dV < 0) {
				RigidBody_2D * A = c.pair[0], *B = c.pair[1];

				// 1/(m1+m2),  coef of restitution, epsilon
				const float Msum_1 = 1.f / (A->m + B->m);
				const float Cr = 0.5f * (A->e + B->e);

				// 2D momentum transfer solution (https://en.wikipedia.org/wiki/Inelastic_collision)
				vec2 pA = A->GetMomentum();
				vec2 pB = B->GetMomentum();
				vec2 pSum = pA + pB;
				vec2 Cr_diff = Cr*(B->V - A->V);
				vec2 v1f = Msum_1*(B->m*Cr_diff + pSum);
				vec2 v2f = Msum_1*(-A->m*Cr_diff + pSum);

				// rotation radius arms
				vec2 r0 = c.pos[0] - A->C;
				vec2 r1 = c.pos[1] - B->C;

				// one of these has to be negated
				v1f = glm::reflect(v1f, -c.normal);
				v2f = glm::reflect(v2f, c.normal);

				// Apply new velocity along reflection direction
				A->ChangeVel(-v1f, r0);
				B->ChangeVel(v2f, r1);

				// I don't like doing this
				c.isColliding = true;
			}
		}
	}
}