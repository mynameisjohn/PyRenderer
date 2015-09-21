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

struct Contact;

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

	// Will have to be more abstract
	std::list<Contact> GetClosestPoints(const Circle& other);
};

struct AABB {
	AABB();
	vec2 C; // Center point of box
	vec2 R; // half widths along x, y
	vec2 V; // velocity

			// TODO consider moving mass, elasticity
			// and collide to base collision primitive
	float M;// mass
	float E;// elasticity

	float width();
	float height();
	float left();
	float right();
	float top();
	float bottom();
	float dX(AABB& other);
	float dY(AABB& other);
	float dist(AABB& other);
	bool overlaps(AABB& other);
	void advance(float dt = 0.001f);
	void translate(vec2 d);
	vec2 momentum();
	float kinetic();
	bool collide(AABB& other);
	// TODO rotate func, Ericson 86
};