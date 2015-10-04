#include <glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

RigidBody_2D::RigidBody_2D() :
	V(0),
	C(0),
	m(1),
	e(1),
	th(0),
	w(0)
{}


RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e) :
	V(V),
	C(C),
	m(m),
	e(e),
	th(0),
	w(0)
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

void RigidBody_2D::ChangeVel(vec2 newV, vec2 rad) {
	vec2 delV = newV - V;
	V = newV;
	vec2 perp(-rad.y, rad.x);
	th += glm::dot(delV, perp);
}

void AABB::ChangeVel(vec2 newV, vec2 rad) {
	V = newV;
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
	// d points from the circle to the box, so negate and clamp
	vec2 b_pos = other.clamp(C);
	vec2 n = glm::normalize(b_pos - C);

	vec2 a_pos = C + r*n;
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}


bool Circle::IsOverlapping(const OBB& other) const {
	return other.IsOverlapping(*this);
}

std::list<Contact> Circle::GetClosestPoints(const OBB& other) const {
	return other.GetClosestPoints(*this);
}

bool AABB::IsOverlapping(const OBB& other) const {
	return other.IsOverlapping(*this);
}

std::list<Contact> AABB::GetClosestPoints(const Circle& other) const {
	return other.GetClosestPoints(*this);
}

std::list<Contact> AABB::GetClosestPoints(const OBB& other) const {
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

// // Impuse application methods
//static void ApplyCollisionImpulse_Generic(RigidBody_2D * a, RigidBody_2D * b, vec2 N) {
//	// Solve 1-D collision along normal between objects
//	glm::vec2 n = glm::normalize(a->C - b->C);
//	float v1i = glm::dot(n, a->V);
//	float v2i = glm::dot(n, b->V);
//
//	// 1/(m1+m2),  coef of restitution
//	const float Msum_1 = 1.f / (a->m + b->m);
//	const float Cr = 0.5f * (a->e + b->e);
//
//	// find momentum, solve for final velocity
//	float pa = a->m * v1i, pb = b->m * v2i;
//	float Cr_diff = Cr*(v2i - v1i);
//	float psum = pa + pb;
//
//	float v1f = Msum_1*(b->m*Cr_diff + psum);
//	float v2f = Msum_1*(-a->m*Cr_diff + psum);
//
//	// do this more cute (needed for "conservation of momentum")
//	if (fabs(v1f) < 0.001f) v1f = 0.f;
//	if (fabs(v2f) < 0.001f) v2f = 0.f;
//
//	std::cout << "p0: " << pa + pb << ", p1: " << v1f*a->m + v2f*b->m << std::endl;
//
//	// apply velocity along normal direction
//	a->V = v1f*n;
//	b->V = v2f*n;
//}
//
//void RigidBody_2D::ApplyCollisionImpulse(RigidBody_2D * const other, vec2 n) {
//	ApplyCollisionImpulse_Generic(this, other, n);
//}



OBB::OBB():
	AABB()
{}

OBB::OBB(const AABB& ab) :
	AABB(ab)
{}

OBB::OBB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r, float th) :
	AABB(vel, c, mass, elasticity, r)
{}

OBB::OBB(vec2 vel, float m, float e, float x, float y, float w, float h, float th) :
	AABB(vel, m, e, x,y,w, h)
{}

glm::mat2 OBB::getRotMat() const {
	float c = cos(th);
	float s = sin(th);
	return glm::mat2(vec2(-c, s), vec2(s, c));
}

int OBB::GetSupportVerts(vec2 n, std::array<vec2, 2> sV) const {
	// This isn't necessary
	int found(-1);
	int num(0);
	vec2 l_n = glm::inverse(getRotMat()) * n;

findMin:
	float dMin(-FLT_MAX);
	vec2 vMin(-FLT_MAX);

	for (int i = 0; i < 4; i++) {
		vec2 v = GetVert(i);
		float d = glm::dot(l_n, v);
		if (d < dMin) {
			if (found != i) {
				vMin = v;
				found = i;
			}
		}
	}

	sV[num++] = vMin;
	if (num == 1)
		goto findMin;

	return num;
}

int OBB::GetSupportVertIndices(vec2 n, std::array<int, 2> sV) const {
	// This isn't necessary
	int iMin(-1);
	int num(0);
	vec2 l_n = glm::inverse(getRotMat()) * n;

findMin:
	float dMin(-FLT_MAX);

	for (int i = 0; i < 4; i++) {
		vec2 v = GetVert(i);
		float d = glm::dot(l_n, v);
		if (d < dMin) {
			if (i != iMin)
				i = iMin;
		}
	}

	sV[num++] = iMin;
	if (num == 1)
		goto findMin;

	return num;
}

std::array<vec2, 2> OBB::GetSupportPair(vec2 n) const {
	std::array<vec2, 2> ret;

	vec2 va, vb, vc;
	vec2 nab = glm::normalize(vb - va);
	vec2 nbc = glm::normalize(vc - vb);
	if (glm::dot(nab, n) < glm::dot(nbc, n)) {
		ret[0] = va;
		ret[1] = vb;
	}
	else {
		ret[0] = vb;
		ret[1] = vc;
	}

	return ret;
}

vec2 OBB::GetVert(uint32_t idx) const {
	vec2 ret(0);
	switch (idx % 4) {
	case 0:
		return C + getRotMat() *  R;
	case 1:
		return C + getRotMat() * vec2(R.x, -R.y);
	case 2:
		return C - getRotMat() * R;
	case 3:
	default:
		return C + getRotMat() * vec2(-R.x, R.y);
	}
}

vec2 OBB::GetNormal(uint32_t idx) const {
	switch (idx % 4) {
	case 0:
		return getRotMat() * vec2(1, 0);
	case 1:
		return getRotMat() * vec2(0, -1);
	case 2:
		return getRotMat() * vec2(-1, 0);
	case 3:
	default:
		return getRotMat() * vec2(0, 1);
	}
}

static vec2 projectOnEdge(vec2 p, vec2 e0, vec2 e1) {
	// u and v form a little triangle
	vec2 u = p - e0;
	vec2 v = e1 - e0;

	// find the projection of u onto v, parameterize it
	float t = glm::dot(u, v) / glm::length(v);

	// Clamp between two edge points
	return glm::clamp(e0 + e1 * t, e0, e1);
}


std::list<Contact> OBB::GetClosestPoints(const OBB& other) const {
	// Move this elsewhere once triangles come into play
	struct FeaturePair {
		enum Type {
			F_V,
			V_F,
			N
		};
		float dist{ -FLT_MAX };
		float c_dist{ -FLT_MAX };
		int fIdx{ -1 };
		int vIdx{ -1 };
		Type T{ Type::N };
	};

	std::list<Contact> ret;
	FeaturePair mostSeparated;
	FeaturePair mostPenetrating;

	auto faceVert = [&mostSeparated, &mostPenetrating](OBB * A, OBB * B, FeaturePair::Type type) {
		// Smol
		const float kEPS = 0.001f;

		// For all A's normals
		for (int i = 0; i < 4; i++) {
			vec2 n = A->GetNormal(i);
			vec2 p1 = A->GetVert(i);
			vec2 p2 = A->GetVert((i + 1) % 4);

			// For B's support verts relative to the normal
			std::array<int, 2> supportVerts;
			int nVerts = B->GetSupportVertIndices(n, supportVerts);
			for (int s = 0; s < nVerts; s++) {
				vec2 sV = B->GetVert(supportVerts[s]);

				// minkowski face points
				vec2 mfp0 = sV - p1;
				vec2 mfp1 = sV - p2;

				// Find point on minkowski face
				vec2 p = projectOnEdge(vec2(), mfp0, mfp1);

				// are objects penetrating?
				// i.e is first mf point behind face normal
				// penetration implies support vert behind normal
				float dist = glm::dot(mfp0, n);
				bool isPenetrating = dist < 0.f;
				FeaturePair * fp = isPenetrating ? &mostPenetrating : &mostSeparated;
				// Pick the closest one
				if (isPenetrating == false)
					dist = glm::length(p);
				float c_dist = glm::length(sV - A->C);
				float del = fabs(dist - fp->dist);
				bool overwrite = (del < kEPS &&  fp->T == type && c_dist < fp->c_dist) || (dist < fp->dist);
				if (overwrite) {
					fp->dist = dist;
					fp->c_dist = c_dist;
					fp->fIdx = i;
					fp->vIdx = supportVerts[s];
					fp->T = type;
				}
			}
		}
	};

	// Check our faces with their verts, then vice versa
	faceVert((OBB *)this, (OBB *)&other, FeaturePair::Type::F_V);
	faceVert((OBB *)&other, (OBB *)this, FeaturePair::Type::V_F);

	// Pick the correct feature pair
	bool pen = (mostSeparated.dist > 0);
	FeaturePair * fp = pen ? &mostPenetrating : &mostSeparated;
	
	auto makePoints = [fp, &ret](OBB * A, OBB * B){
		std::array<vec2, 4> contactPoints;

		vec2 wN = A->GetNormal(fp->fIdx);
		vec2 wE0 = A->GetVert(fp->fIdx);
		vec2 wE1 = A->GetVert(fp->fIdx + 1);

		vec2 wV0 = B->GetVert(fp->vIdx);
		vec2 wV1 = B->GetVert(fp->vIdx + 1);

		vec2 p1 = projectOnEdge(wE0, wV0, wV1);
		vec2 p2 = projectOnEdge(wE1, wV0, wV1);

		vec2 p3 = projectOnEdge(wV0, wE0, wE1);
		vec2 p4 = projectOnEdge(wV1, wE0, wE1);

		ret.emplace_back(A, B, p1, p3, wN, glm::length(p3 - p1));
		ret.emplace_back(A, B, p2, p4, wN, glm::length(p4 - p2));
	};

	if (fp->T == FeaturePair::Type::F_V)
		makePoints((OBB *)this, (OBB *)&other);
	else
		makePoints((OBB *)&other, (OBB *)this);

	return ret;
}

// Not sure about this
std::list<Contact> OBB::GetClosestPoints(const Circle& other) const {
	vec2 a_pos = ws_clamp(other.C);
	vec2 n = glm::normalize(a_pos - other.C);
	vec2 b_pos = other.C + n*other.r;
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

vec2 OBB::ws_clamp(vec2 p) const {
	vec2 l_p = glm::inverse(getRotMat()) * p;
	return AABB::clamp(l_p);
}

std::list<Contact> OBB::GetClosestPoints(const AABB& other) const {
	// This is lazy
	OBB ob(other);
	return GetClosestPoints(ob);
}

// NYI
bool OBB::IsOverlapping(const Circle& other) const {
	return false;
}

bool OBB::IsOverlapping(const AABB& other) const {
	return false;
}

quatvec RigidBody_2D::GetQuatVec() const {
	return quatvec(vec3(C, 0.f), fquat(cos(th / 2), sin(th / 2)*vec3(0, 0, 1)));
}