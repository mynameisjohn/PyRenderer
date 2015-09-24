#pragma once

#include <vec2.hpp>

#include "Util.h"
#include "Entity.h"

// Contact forward
struct Contact;

// Forward all primitives, and make RB2D abstractly support all
struct Circle;
struct AABB;

// I don't know if the base collision primitive should be a pycomponent
struct RigidBody_2D : public PyComponent {
	vec2 V; // Velocity
	vec2 C; // Center point
	float m; // Mass
	float e; // elasticity
	vec2 lastT; // Last translation (doesn't belong here)
	RigidBody_2D(vec2 V = vec2(0), vec2 C = vec2(0), float m = 1.f, float e = 1.f, Entity * pEnt = nullptr);

	// Overlap
	virtual bool IsOverlapping(const Circle& other) const = 0;
	virtual bool IsOverlapping(const AABB& other) const = 0;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const = 0;

	// Applying collision between objects
	virtual void ApplyCollisionImpulse(RigidBody_2D * const other) = 0;

	// Integrate velocity over time
	void Integrate();

	// Python overrides, Not really primitive specific
	bool PyExpose(const std::string& name, PyObject * module) override;
	bool PyUpdate() override;
};

struct Circle : public RigidBody_2D {
	float r; // Radius

	Circle(vec2 V = vec2(0), vec2 C = vec2(0), float m = 1.f, float e = 1.f, float radius = 1.f, Entity * pEnt = nullptr);

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;

	// Applying collision between objects
	// This is a really bad idea because it matters whom gets applied to whom, 
	// i.e a box can collide with a circle, but a circle can't collide with a box (correctly)
	void ApplyCollisionImpulse(RigidBody_2D * const other) override;
};

struct AABB : public RigidBody_2D {
	vec2 R; // half widths along x, y

	AABB(vec2 V = vec2(0), float m = 1.f, float e = 1.f, float x = 0.f, float y = 0.f, float w = 1.f, float h = 1.f, Entity * pEnt = nullptr);

	// useful things
	float width() const;
	float height() const;
	float left() const;
	float right() const;
	float top()const;
	float bottom()const;
	vec2 clamp(vec2 p) const;

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;

	// Applying collision between objects
	void ApplyCollisionImpulse(RigidBody_2D * const other) override;
};