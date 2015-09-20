#pragma once

#include <vec2.hpp>

#include "Util.h"
#include "Entity.h"

struct RigidBody_2D {
	vec2 V; // Velocity
	vec2 C; // Center point
	float m; // Mass
	float e; // elasticity
	vec2 lastT; // Last translation (doesn't belong here)
	RigidBody_2D() { memset(this, 0, sizeof(RigidBody_2D)); }
	RigidBody_2D(vec2 V, vec2 C, float m, float e);
};

struct Circle : public RigidBody_2D, public PyComponent {
	Circle() { memset(this, 0, sizeof(Circle)); }
	Circle(vec2 V, vec2 C, float m, float e, float r, Entity * pEnt);

	float r; // Radius
	void Update();

	// Python overrides
	bool PyExpose(const std::string& name, PyObject * module) override;
	bool PyUpdate() override;

	// TODO make these abstract in rb
	bool IsColliding(Circle& other);
	void ApplyCollision(Circle& other);
};
