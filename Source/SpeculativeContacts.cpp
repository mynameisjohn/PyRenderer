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
	pair[0]->V += I;
	pair[1]->V += I;
}

vec2 Contact::relVel() const {
	return pair[0]->V - pair[1]->V;
}

void Solver::operator()(std::list<Contact>& contacts) {
	const float dT(0.05f);
	for (auto& c : contacts) {
		// Find the relative velocity of the system along the normal
		float relNv = glm::dot(c.normal, c.relVel());

		// Find the amount of velocity to remove, along the normal, such that the objects touch

		// Find the amount of velocity (along n) needed such that
		// adding dV * dT makes the objects touch
		float dV = relNv + c.dist / dT;

		// If that value was negative, the objects will penetrate in the next frame
		if (dV < 0) {
			// find the magnitude of the collision impulse (fn of relative velocity, mass)
			float mag = dV / (1.f / c.pair[0]->m + 1.f / c.pair[1]->m); //yeesh

			c.ApplyImpulse(mag);
		}
	}
}