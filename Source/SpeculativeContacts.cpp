#include <glm.hpp>

#include "SpeculativeContacts.h"
#include "RigidBody.h"

Contact::Contact(RigidBody_2D * a, RigidBody_2D * b, const vec2 p_a, const vec2 p_b, const vec2 nrm, const float i, const float d) :
	pair{ a, b },
	pos{ p_a,p_b },
	normal(nrm),
	impulse(i),
	dist(d)
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

	for (int i = 0; i < m_nIterations; i++)
	{
		for (auto& c : contacts) {
			// Find the relative velocity of the system along the normal
			float relNv = glm::dot(c.normal, c.relVel());

			// Find the amount of velocity to remove, along the normal, such that the objects touch

			// Find the amount of velocity (along n) needed such that
			// adding dV * dT makes the objects touch
			float dV = relNv + c.dist / dT;

			if (dV < 0) {
				// 1/(m1+m2),  coef of restitution, epsilon
				const float Msum_1 = 1.f / (c.pair[0]->m + c.pair[1]->m);
				const float Cr = 0.5f * (c.pair[0]->e + c.pair[1]->e);
				const float EPS = 0.001f;

				// 2D momentum transfer solution (https://en.wikipedia.org/wiki/Inelastic_collision)
				vec2 pA = c.pair[0]->GetMomentum();
				vec2 pB = c.pair[1]->GetMomentum();
				vec2 pSum = pA + pB;
				vec2 Cr_diff = Cr*(c.pair[0]->V - c.pair[1]->V);
				vec2 v1f = Msum_1*(c.pair[1]->m*Cr_diff + pSum);
				vec2 v2f = Msum_1*(c.pair[0]->m*Cr_diff + pSum);

				// Apply new velocity along reflection direction
				c.pair[0]->V = glm::reflect(v1f, c.normal);
				c.pair[1]->V = glm::reflect(v2f, c.normal);

				// Check conservation of Energy
				std::cout << c.pair[0]->GetKineticEnergy() << "\n" << c.pair[1]->GetKineticEnergy() << "\n" << std::endl;
			}
		}
	}
}