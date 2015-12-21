#pragma once

#include <glm/vec2.hpp>

#include "Util.h"
#include "Entity.h"
#include "quatvec.h"

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
struct Capsule;
struct Lozenge;

// I don't know if the base collision primitive should be a pycomponent
struct RigidBody_2D : public OwnedByEnt {
	vec2 V; // Velocity
	vec2 C; // Center point
	float m; // Mass
	float e; // elasticity
	float th; // rotation
	float w; // angular velocity

	RigidBody_2D();
	RigidBody_2D(vec2 vel, vec2 c, float mass, float elasticity, float th=0.f);

	// Get Physical Quantities
	vec2 GetMomentum() const;
	float GetKineticEnergy() const;
    virtual float GetInertia() const = 0;

	// Overlap, everyone needs to be compatible
	virtual bool IsOverlapping(const Circle& other) const = 0;
	virtual bool IsOverlapping(const AABB& other) const = 0;
	virtual bool IsOverlapping(const OBB& other) const = 0;
	virtual bool IsOverlapping(const Capsule& other) const = 0;
	virtual bool IsOverlapping(const Lozenge& other) const = 0;

	// Contacts (TODO pass a container around by reference)
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const OBB& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const Capsule& other) const = 0;
	virtual std::list<Contact> GetClosestPoints(const Lozenge& other) const = 0;

	// Applying collision between objects
	//void ApplyCollisionImpulse(RigidBody_2D * const other, vec2 n);

	// Might phase out above
	virtual void ChangeVel(vec2 newV, vec2 rad);

	// Integrate velocity over time (virt for AABB)
	virtual void Integrate();

	// Get rotation/translation
	quatvec GetQuatVec() const;

	glm::mat2 getRotMat() const;
};


// 2D Circle
struct Circle : public RigidBody_2D {
	float r; // Radius

	Circle();
	Circle(vec2 vel, vec2 c, float mass, float elasticity, float radius, float th=0.f);
    
    float GetInertia() const override;

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;
	bool IsOverlapping(const OBB& other) const override;
	bool IsOverlapping(const Capsule& other) const override;
	bool IsOverlapping(const Lozenge& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;
	std::list<Contact> GetClosestPoints(const OBB& other) const override;
	std::list<Contact> GetClosestPoints(const Capsule& other) const override;
	std::list<Contact> GetClosestPoints(const Lozenge& other) const override;
};

// 2D Axis Aligned Bounding Box
struct AABB : public RigidBody_2D {
	vec2 R; // half widths along x, y

	AABB();
	AABB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r);
	AABB(vec2 vel, float m, float e, float x, float y, float w, float h);
    
    float GetInertia() const override;

	// useful things
	float width() const;
	float height() const;
	float left() const;
	float right() const;
	float top() const;
	float bottom() const;
	vec2 clamp(vec2 p) const;
    
    vec2 GetFaceNormalFromPoint(vec2 p) const;
    
    // Override integrate to null out rotation
    void Integrate() override;

	// Collision Overrides
	virtual bool IsOverlapping(const Circle& other) const override;
	virtual bool IsOverlapping(const AABB& other) const override;
	virtual bool IsOverlapping(const OBB& other) const override;
	virtual bool IsOverlapping(const Capsule& other) const override;
	virtual bool IsOverlapping(const Lozenge& other) const override;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const override;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const override;
	virtual std::list<Contact> GetClosestPoints(const OBB& other) const override;
	virtual std::list<Contact> GetClosestPoints(const Capsule& other) const override;
	virtual std::list<Contact> GetClosestPoints(const Lozenge& other) const override;

	// No rotation applied
	void ChangeVel(vec2 newV, vec2 rad) override;

	virtual vec2 GetVert(uint32_t idx) const;
	virtual vec2 GetNormal(uint32_t idx) const;
};

// 2D Oriented Bounding Box (should this inherit from AABB?)
struct OBB : public AABB {
	// Constructors like AABB, with rotation
	OBB();
	OBB(const AABB& ab);
	OBB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r, float th = 0.f);
	OBB(vec2 vel, float m, float e, float x, float y, float w, float h, float th = 0.f);

	// Rather than override these, keep the originals and
	// then make new versions for world space
	//float ws_width() const;
	//float ws_height() const;
	//float ws_left() const;
	//float ws_right() const;
	//float ws_top() const;
	//float ws_bottom() const;
	vec2 ws_clamp(vec2 p) const;

	vec2 GetVert(uint32_t idx) const override;
	vec2 GetNormal(uint32_t idx) const override;
    
    // Cancel AABB's override (this is stupid
    void Integrate() override;

	// Collision Overrides
	virtual bool IsOverlapping(const Circle& other) const override;
	virtual bool IsOverlapping(const AABB& other) const override;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const override;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const override;
	virtual std::list<Contact> GetClosestPoints(const OBB& other) const override;
	
	int GetSupportVerts(vec2 n, std::array<vec2, 2>&) const;
	int GetSupportIndices(vec2 n, std::array<int, 2>&) const;
	std::array<vec2, 2> GetSupportNeighbor(vec2 n, int idx) const;
    vec2 GetSupportNormal(vec2 N) const;

	// AABB fucks it up
	void ChangeVel(vec2 newV, vec2 rad) override;
};

struct Capsule : public RigidBody_2D {
	float r;	// Circle radius
	float L;	// length between pA, pB

	Capsule();
	Capsule(vec2 vel, vec2 c, float mass, float elasticity, vec2 s, float th = 0.f);

	// Collision Overrides
	bool IsOverlapping(const Circle& other) const override;
	bool IsOverlapping(const AABB& other) const override;
	bool IsOverlapping(const OBB& other) const override;
	bool IsOverlapping(const Capsule& other) const override;
	bool IsOverlapping(const Lozenge& other) const override;

	// Contacts
	std::list<Contact> GetClosestPoints(const Circle& other) const override;
	std::list<Contact> GetClosestPoints(const AABB& other) const override;
	std::list<Contact> GetClosestPoints(const OBB& other) const override;
	std::list<Contact> GetClosestPoints(const Capsule& other) const override;
	std::list<Contact> GetClosestPoints(const Lozenge& other) const override;

	std::array<vec2, 2> GetEndpoints() const;
};

struct Lozenge : public RigidBody_2D {
	float r;	// Circle radius
	vec2 R;		// box dims

	Lozenge();
	Lozenge(vec2 vel, vec2 c, float mass, float elasticity, vec2 s, float th = 0.f);

	// This controls roundness - hardcode for now
	const float ALPHA = 0.5f;

	// Collision Overrides
	virtual bool IsOverlapping(const Circle& other) const override;
	virtual bool IsOverlapping(const AABB& other) const override;
	virtual bool IsOverlapping(const OBB& other) const override;
	virtual bool IsOverlapping(const Capsule& other) const override;
	virtual bool IsOverlapping(const Lozenge& other) const override;

	// Contacts
	virtual std::list<Contact> GetClosestPoints(const Circle& other) const override;
	virtual std::list<Contact> GetClosestPoints(const AABB& other) const override;
	virtual std::list<Contact> GetClosestPoints(const OBB& other) const override;
	virtual std::list<Contact> GetClosestPoints(const Capsule& other) const override;
	virtual std::list<Contact> GetClosestPoints(const Lozenge& other) const override;
};

// Feature Pair definition; does this belong here?

// Move this elsewhere once triangles come into play
struct FeaturePair {
    enum Type {
        F_V,
        V_F,
        N
    };
    float dist;
    float c_dist;
    int fIdx;
    int vIdx;
    Type T;
    FeaturePair(float d, float cd = -1, int f = -1, int v = -1, Type t = N);
};