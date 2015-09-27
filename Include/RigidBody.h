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
struct RigidBody_2D : public OwnedByEnt {
	vec2 V; // Velocity
	vec2 C; // Center point
	float m; // Mass
	float e; // elasticity

	RigidBody_2D();
	RigidBody_2D(vec2 vel, vec2 c, float mass, float elasticity);

	// Get Physical Quantities
	vec2 GetMomentum() const;
	float GetKineticEnergy() const;

	// Overlap, everyone needs to be compatible
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

	Circle();
	Circle(vec2 vel, vec2 c, float mass, float elasticity, float radius);

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;
};

struct AABB : public RigidBody_2D {
	vec2 R; // half widths along x, y

	AABB();
	AABB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r);
	AABB(vec2 vel, float m, float e, float x, float y, float w, float h);

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