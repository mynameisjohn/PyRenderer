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
	RigidBody_2D(vec2 V=vec2(0), vec2 C = vec2(0), float m=1.f, float e=1.f);
};

struct Circle : public RigidBody_2D, public PyComponent {
	Circle(vec2 V = vec2(0), vec2 C = vec2(0), float m = 1.f, float e = 1.f, float r = 1.f, Entity * pEnt = nullptr);

	float r; // Radius
	void Update();

	// Python overrides
	bool PyExpose(const std::string& name, PyObject * module) override;
	bool PyUpdate() override;

	// TODO make these abstract in rb
	bool IsColliding(Circle& other);
	void ApplyCollision(Circle& other);
};
