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
struct RigidBody_2D {
	vec2 V; // Velocity
	vec2 C; // Center point
	float m; // Mass
	float e; // elasticity
	vec2 lastT; // Last translation (doesn't belong here)
	RigidBody_2D(vec2 V = vec2(0), vec2 C = vec2(0), float m = 1.f, float e = 1.f);

	// Get Physical Quantities
	vec2 GetMomentum() const;
	float GetKineticEnergy() const;

	// Overlap
	virtual bool IsOverlapping(const Circle& other) const = 0;
	virtual bool IsOverlapping(const AABB& other) const = 0;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const = 0;

	// Applying collision between objects
	void ApplyCollisionImpulse(RigidBody_2D * const other, vec2 n);

	// Integrate velocity over time
	void Integrate();
};

struct Circle : public RigidBody_2D {
	float r; // Radius

	Circle(vec2 V = vec2(0), vec2 C = vec2(0), float m = 1.f, float e = 1.f, float radius = 1.f;

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;
};

struct AABB : public RigidBody_2D {
	vec2 R; // half widths along x, y

	AABB(vec2 V = vec2(0), float m = 1.f, float e = 1.f, float x = 0.f, float y = 0.f, float w = 1.f, float h = 1.f);

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
};