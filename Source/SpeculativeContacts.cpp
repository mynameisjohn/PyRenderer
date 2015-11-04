#include <glm/glm.hpp>

#include "SpeculativeContacts.h"
#include "RigidBody.h"
#include "Audible.h"

Contact::Contact(RigidBody_2D * a, RigidBody_2D * b, const vec2 p_a, const vec2 p_b, const vec2 nrm, const float d) :
	pair{ a, b },
	pos{ p_a, p_b },
	normal(nrm),
	dist(d),
	curImpulse(0),
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

// Apply a 2D impulse 
void Contact::ApplyImpulse(float mag) {
	vec2 imp = normal * mag;
	for (int i = 0; i < 2; i++) {
		const float sgn = i % 2 ? -1.f : 1.f;
		pair[i]->V += sgn * imp / pair[i]->m;
		pair[i]->w += sgn * glm::dot(imp, rad[i]) / pair[i]->GetInertia();
	}

	// std::cout << A->GetKineticEnergy() + B->GetKineticEnergy() << std::endl;
}

inline vec2 contactVel(const Contact * c, int i) {
	return c->pair[i]->V + c->rad[i] * c->pair[i]->w;
}

vec2 Contact::GetVel_A() const {
	return contactVel(this, 0);
}

vec2 Contact::GetVel_B() const {
	return contactVel(this, 1);
}

float Contact::GetVelN_A() const {
	return glm::dot(GetVel_A(), normal);
}

float Contact::GetVelN_B() const {
	return glm::dot(GetVel_B(), normal);
}

// Detect and solve collisions, iteratively
uint32_t Solver::Solve(std::list<Contact>& contacts) {
	uint32_t numCollisions(0);

	for (int nIt = 0; nIt < m_nIterations; nIt++)
	{
		uint32_t colCount = 0;
		for (auto& c : contacts) {
			// Coeffcicient of restitution, plus 1
			const float Cr_1 = 1.f + 0.5f * (c.pair[0]->e + c.pair[1]->e);

			// Get the relative contact velocity - Vb - Va
			float vA_N = c.GetVelN_A();
			float vB_N = c.GetVelN_B();
			float relNv = vB_N - vA_N;

			// Find out how much you'd remove to have them just touch
			float remove = relNv + c.dist / globalTimeStep;

			// Detect collision
			bool colliding = (remove < -kEPS);
			if (colliding) {
				c.isColliding = true;
				colCount++;
			}

			// Get the magnitude of the collision (negative or zero)
			float impulseMag = colliding ? (Cr_1 * relNv * c.invMassI) : 0.f;

			// Calculate the new impulse and apply the change
			float n_Impulse = impulseMag + c.curImpulse;
			float delImpulse = n_Impulse - c.curImpulse;
			c.ApplyImpulse(delImpulse);

			// Store new impulse
			c.curImpulse = n_Impulse;
		}

		// Maybe break
		if (colCount == 0)
			break;
		else
			numCollisions += colCount;
	}

	return numCollisions;
}

uint32_t Solver::operator()(std::list<Contact>& contacts) {
	return Solve(contacts);
}