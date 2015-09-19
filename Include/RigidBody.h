#pragma once

#include "Util.h"
#include <vec2.hpp>

struct RigidBody_2D {
	vec2 V; // Velocity
	vec2 C; // Center point
	float m; // Mass
	float e; // elasticity
};

struct Circle : public RigidBody_2D {
	float r; // Radius
};
