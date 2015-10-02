#include <glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

RigidBody_2D::RigidBody_2D() :
	V(0),
	C(0),
	m(1),
	e(1)
{}


RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e) :
	V(V),
	C(C),
	m(m),
	e(e)
{}

// I guess this just advances the object?
void RigidBody_2D::Integrate() {
	const float dT = 0.005f; // TODO better integration methods?
	vec2 delta = dT * V;
	C += delta;

	// I used to have python do this, but what's the point?
	m_pEntity->PostMessage(int(Entity::CompID::DRAWABLE),
		int(Entity::MsgID::DR_TRANSLATE));
}

vec2 RigidBody_2D::GetMomentum() const {
	return m * V;
}

float RigidBody_2D::GetKineticEnergy() const {
	return 0.5f * m * glm::dot(V, V);
}

bool Circle::IsOverlapping(const Circle& other) const {
	float dist = glm::length(C - other.C);
	float totalRadius = r + other.r;

	return (dist < totalRadius);
}

bool Circle::IsOverlapping(const AABB& other) const {
	bool x = (other.left() > C.x + r) || (other.right() > C.y - r);
	bool y = (other.bottom() > C.x + r) || (other.top() > C.y - r);
	return x && y;
}

Circle::Circle() :
	r(1),
	RigidBody_2D()
{}

Circle::Circle(vec2 V, vec2 C, float m, float e, float radius) :
	r(radius),
	RigidBody_2D(V, C, m, e)
{}

AABB::AABB() :
	R(1),
	RigidBody_2D()
{}

AABB::AABB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r) :
	R(r),
	RigidBody_2D(vel, c, mass, elasticity)
{}

AABB::AABB(vec2 v, float mass, float el, float x, float y, float w, float h) :
	R(vec2(w, h) / 2.f),
	RigidBody_2D(v, vec2(x, y) + vec2(w, h) / 2.f, mass, el)
{
	R = vec2(w, h) / 2.f;
}

float AABB::width()const { return 2.f*R.x; }
float AABB::height()const { return 2.f*R.y; }
float AABB::left()const { return C.x - R.x; }
float AABB::right()const { return C.x + R.x; }
float AABB::top()const { return C.y + R.y; }
float AABB::bottom()const { return C.y - R.y; }

bool AABB::IsOverlapping(const AABB& other) const {
	return
		(left() > other.right() || right() < other.left()) &&
		(bottom() > other.top() || top() < other.bottom());
}

bool AABB::IsOverlapping(const Circle& other) const {
	return other.IsOverlapping(*this);
}

vec2 AABB::clamp(vec2 p) const {
	return  glm::clamp(p, C - R, C + R);
}

// Closest Point methods

// Kind of a waste
std::list<Contact> Circle::GetClosestPoints(const Circle& other) const {
	// watch for low values here
	// find and normalize distance
	vec2 d = other.C - C;
	vec2 n = glm::normalize(d);
	// contact points along circumference
	vec2 a_pos = C + n*r;
	vec2 b_pos = other.C - n*other.r;
	// distance between circumferences
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

std::list<Contact> Circle::GetClosestPoints(const AABB& other) const {
	// watch for low values here
	//vec2 d = other.C - C;
	//vec2 n = glm::normalize(d);
	//vec2 a_pos = C + n*r;

	// d points from the circle to the box, so negate and clamp
	vec2 b_pos = other.clamp(C);
	vec2 n = glm::normalize(b_pos - C);


	vec2 a_pos = C + r*n;
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

std::list<Contact> AABB::GetClosestPoints(const Circle& other) const {
	return other.GetClosestPoints(*this);
}

std::list<Contact> AABB::GetClosestPoints(const AABB& other) const {
	// watch for low values here
	vec2 d = C - other.C;
	vec2 n = glm::normalize(d);
	vec2 a_pos = clamp(d);
	vec2 b_pos = other.clamp(d);
	float dist = glm::length(a_pos - b_pos);
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

// Impuse application methods
static void ApplyCollisionImpulse_Generic(RigidBody_2D * a, RigidBody_2D * b, vec2 N) {
	// Solve 1-D collision along normal between objects
	glm::vec2 n = glm::normalize(a->C - b->C);
	float v1i = glm::dot(n, a->V);
	float v2i = glm::dot(n, b->V);

	// 1/(m1+m2),  coef of restitution
	const float Msum_1 = 1.f / (a->m + b->m);
	const float Cr = 0.5f * (a->e + b->e);

	// find momentum, solve for final velocity
	float pa = a->m * v1i, pb = b->m * v2i;
	float Cr_diff = Cr*(v2i - v1i);
	float psum = pa + pb;

	float v1f = Msum_1*(b->m*Cr_diff + psum);
	float v2f = Msum_1*(-a->m*Cr_diff + psum);

	// do this more cute (needed for "conservation of momentum")
	if (fabs(v1f) < 0.001f) v1f = 0.f;
	if (fabs(v2f) < 0.001f) v2f = 0.f;

	std::cout << "p0: " << pa + pb << ", p1: " << v1f*a->m + v2f*b->m << std::endl;

	// apply velocity along normal direction
	a->V = v1f*n;
	b->V = v2f*n;
}

void RigidBody_2D::ApplyCollisionImpulse(RigidBody_2D * const other, vec2 n) {
	ApplyCollisionImpulse_Generic(this, other, n);
}

std::list<Contact> OBB::GetClosestPoints(const OBB& other) const {
	struct FeaturePair {
		enum Type {
			F_V,
			V_F,
			V_V,
			F_F
		};
		float dist;
		vec2 pos;
		Type m_Type;
	};

	std::list<Contact> ret;
	FeaturePair mostSeparated, mostPenetrating;

	// For all of my normals
	for (int i = 0; i < 4; i++) {
		vec2 n(0);
		vec2 p1(0), p2(0);
		glm::mat2 rot = getRotMat();

		switch (i) {
		case 0: // p1 = top right
			n = rot * vec2(1, 0);
			p1 = rot * R;
			p2 = rot * vec2(R.x, -R.y);
			break;
		case 1: // p1 = bottom right
			n = getRotMat() * vec2(0, -1);
			p1 = rot * vec2(R.x, -R.y);
			p2 = rot * -R;
			break;
		case 2: // p1 = bottom left
			n = getRotMat() * vec2(-1, 0);
			p1 = rot * -R;
			p2 = rot * vec2(-R.x, R.y);
			break;
		case 3: // p1 = top left
			n = getRotMat() * vec2(0, 1);
			p1 = rot * vec2(-R.x, R.y);
			p2 = rot * R;
			break;
		}

		// For their two support verts relative to the normal
		std::array<vec2, 2> supportVerts;
		int nVerts = other.GetSupportVerts(n, supportVerts);
		for (int i = 0; i < nVerts; i++) {
			vec2 sV = supportVerts[i];

			// minkowski face points
			vec2 mfp0 = sV - p1;
			vec2 mfp1 = sV - p2;

			// are objects penetrating?
			// i.e is first mf point behind face normal?
			// Why first point? Arbitrary, I assume we get to all eventually
			// Why support verts? They're the best candidates for being 'in front'
			// of the normal, so if they're behind then so is everyone else
			// So penetration implies support vert behind normal
			bool isPenetrating = glm::dot(mfp0, n) < 0.f;
			if (isPenetrating) {
				// Project origin onto minkowski face/edge/thing
				vec2 v = -mfp0; // vec2(0) - mfp0, imagine a triangle
				vec2 E = mfp1 - mfp0;

				// parametrize along edge, clamp
				float t = glm::dot(E, v) / glm::dot(E, E);
				vec2 p = glm::clamp(mfp0 + E*t, mfp0, mfp1);
				float dist = glm::length(p);
				if (dist < mostSeparated.dist)
					mostSeparated = FeaturePair(); // fill in correct values
				// add another check to see if it's equal-ish
			}
			else {
				// Do the same thing with mostPenetrating
			}
		}
	}

	// Then do same for reverse (my verts, their normals)
	// ...

	// See which was better
	FeaturePair featureToUse;
	if (mostSeparated.dist > 0)
		featureToUse = mostSeparated;
	else
		featureToUse = mostPenetrating;

	// Now that we know what feature pair we care about
	// get normal of face feature
	vec2 faceN;

	// get vertex pair of vert feature and it's closest neighbor
	// closest neighbor is determined by picking the vertex pair
	// whose edge normal is most opposed to the face feature box's normal
	std::array<vec2, 2> supportPair;
	vec2 va, vb, vc;
	vec2 nab = glm::normalize(vb - va);
	vec2 nbc = glm::normalize(vc - vb);
	if (glm::dot(nab, faceN) < glm::dot(nbc, faceN)) {
		supportPair[0] = va;
		supportPair[1] = vb;
	}
	else {
		supportPair[0] = vb;
		supportPair[1] = vc;
	}

	// now find the actual closest points and make the contacts (two)
}