#pragma once

#include <vec2.hpp>

#include "Util.h"
#include "Entity.h"

// This file has colliison primitives that inherit from
// RigidBody_2D. I think this is going to screw me over sooner
// rather than later. I don't know if they really need pointers to
// their entity, although I need some way of tying back colliding objects
// to their entity. In the future I think Collision Components will be composed
// of multiple collision primitives, so it may be less of an issue. Some of
// it may even get moved to a Physics Component, though I'm not really sure

// Contact forward
struct Contact;

// Forward all primitives, and make RB2D abstractly support all
// Less than ideal, but what can you do?
struct Circle;
struct AABB;
struct OBB;

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
	virtual bool IsOverlapping(const OBB& other) const = 0;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const OBB& other) const = 0;

	// Applying collision between objects
	void ApplyCollisionImpulse(RigidBody_2D * const other, vec2 n);

	// Integrate velocity over time
	void Integrate();
};


// 2D Circle
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

// 2D Axis Aligned Bounding Box
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
	float top() const;
	float bottom() const;
	vec2 clamp(vec2 p) const;

	// Collision Overrides
	virtual bool IsOverlapping(const Circle& other) const override;
	virtual bool IsOverlapping(const AABB& other) const override;
	virtual bool IsOverlapping(const OBB& other) const override;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const override;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const override;
	virtual std::list<Contact> GetClosestPoints(const OBB& other) const override;
};

// 2D Oriented Bounding Box (should this inherit from AABB?)
struct OBB : public AABB {
	float theta; // rotation about z axis
	
	// Constructors like AABB, with rotation
	OBB();
	OBB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r, float th);
	OBB(vec2 vel, float m, float e, float x, float y, float w, float h, float th);
	OBB(const AABB&);

	// Rather than override these, keep the originals and
	// then make new versions for world space
	float ws_width() const;
	float ws_height() const;
	float ws_left() const;
	float ws_right() const;
	float ws_top() const;
	float ws_bottom() const;
	vec2 ws_clamp(vec2 p) const;
	glm::mat2 getRotMat() const;

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;
	std::list<Contact> GetClosestPoints(const OBB& other) const override;

	int GetSupportVerts(vec2 n, std::array<vec2, 2>) const;
};