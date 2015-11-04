#include <glm/glm.hpp>

#include "SpeculativeContacts.h"
#include "RigidBody.h"
#include "Audible.h"

Contact::Contact(RigidBody_2D * a, RigidBody_2D * b, const vec2 p_a, const vec2 p_b, const vec2 nrm, const float d) :
	pair{ a, b },
	pos{ p_a, p_b },
	//	A(a),
	//    B(b),
	//    pA(p_a),
	//    pB(p_b),
	normal(nrm),
	dist(d),
	curImpulse(0),
	//    curPenDist(0),
	isColliding(false)
{
	assert(a != b);

	float denom(0.f);
	for (int i = 0; i < 2; i++) {
		rad[i] = perp(pos[i] - pair[i]->C);

		float rn = glm::dot(rad[i], normal);
		denom += 1.f / pair[i]->m;
		denom += powf(rn, 2) / pair[i]->GetInertia();
	}

	//assert(denom > kEPS);

	invMassI = 1.f / denom;
}

void Contact::ApplyImpulse(vec2 imp) {
	for (int i = 0; i < 2; i++) {
		const float sgn = i % 2 ? -1.f : 1.f;
		pair[i]->V += sgn * imp / pair[i]->m;
		pair[i]->w += sgn * glm::dot(imp, rad[i]) / pair[i]->GetInertia();
	}

	// std::cout << A->GetKineticEnergy() + B->GetKineticEnergy() << std::endl;
}

inline vec2 relVel(const Contact * c, int i) {
	return c->pair[i]->V + c->rad[i] * c->pair[i]->w;
}

vec2 Contact::GetRelVel_A() const {
	return relVel(this, 0);
}

vec2 Contact::GetRelVel_B() const {
	return relVel(this, 1);
}

float Contact::GetRelVelN_A() const {
	return glm::dot(GetRelVel_A(), normal);
}

float Contact::GetRelVelN_B() const {
	return glm::dot(GetRelVel_B(), normal);
}


uint32_t Solver::Solve(std::list<Contact>& contacts) {
	uint32_t numCollisions(0);

	for (int nIt = 0; nIt < m_nIterations; nIt++)
	{
		uint32_t numColIt(0);
		for (auto& c : contacts) {
			float vA_N = c.GetRelVelN_A();
			float vB_N = c.GetRelVelN_B();
			float relNv = vB_N - vA_N;

			float Cr_1 = 1.f + 0.5f * (c.pair[0]->e + c.pair[1]->e);

			float remove = relNv + c.dist / globalTimeStep;
			float mag = remove < -kEPS ? Cr_1 * relNv * c.invMassI : 0.f;

			float newImpulse = mag + c.curImpulse;
			if (newImpulse > kEPS)
				newImpulse = 0.f;
			float change = newImpulse - c.curImpulse;
			vec2 imp = change * c.normal;
			c.ApplyImpulse(imp);
		}
	}

	return numCollisions;
}