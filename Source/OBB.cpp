#include "RigidBody.h"
#include "SpeculativeContacts.h"

#include <glm/glm.hpp>
#include <glm/mat2x2.hpp>

////////////////////////////////////////////////////////////////////////////////////////
// OBB functions

OBB::OBB() :
	AABB()
{}

OBB::OBB(const AABB& ab) :
	AABB(ab)
{}

OBB::OBB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r, float th) :
	AABB(vel, c, mass, elasticity, r)
{
	this->th = th;
}

OBB::OBB(vec2 vel, float m, float e, float x, float y, float w, float h, float th) :
	AABB(vel, m, e, x, y, w, h)
{
	this->th = th;
}

// Had to be overriden because this is a subclass of AABB (should it be?)
void OBB::Integrate() {
	RigidBody_2D::Integrate();
}

// Construct rotation matrix
glm::mat2 RigidBody_2D::getRotMat() const {
	float c = cos(th);
	float s = sin(th); // glm wants transpose
	return glm::mat2(vec2(c, s), vec2(-s, c));
}

// Get one or two of the vertices furthest along n
int OBB::GetSupportVerts(vec2 n, std::array<vec2, 2>& sV) const {
	int foundIdx(-1);

	// Find the furthest vertex
	float dMin = -FLT_MAX;
	for (int i = 0; i < 4; i++) {
		vec2 v = GetVert(i);
		float d = glm::dot(n, v);
		if (d > dMin) {
			dMin = d;
			sV[0] = v;
			foundIdx = i;
		}
	}

	int num(1);

	// If there's a different vertex
	for (int i = 0; i < 4; i++) {
		if (i == foundIdx)
			continue;
		vec2 v = GetVert(i);
		float d = glm::dot(n, v);
		// That's pretty close...
		if (feq(d, dMin, 100.f * kEPS)) {
			// Take it too
			dMin = d;
			sV[num++] = v;
		}
	}

	return num;
}

// Indices of the vertices returned by the above function
int OBB::GetSupportIndices(vec2 n, std::array<int, 2>& sV) const {
	// Find the furthest vertex
	float dMin = -FLT_MAX;
	for (int i = 0; i < 4; i++) {
		vec2 v = GetVert(i);
		float d = glm::dot(n, v);
		if (d > dMin) {
			dMin = d;
			sV[0] = i;
		}
	}

	int num(1);

	// If there's a different vertex
	for (int i = 0; i < 4; i++) {
		if (i == sV[0])
			continue;
		vec2 v = GetVert(i);
		float d = glm::dot(n, v);
		// That's pretty close...
		if (feq(d, dMin, 100.f * kEPS)) {
			// Take it too
			dMin = d;
			sV[num++] = i;
		}
	}

	return num;
}

// Best face normal aligned with N
vec2 OBB::GetSupportNormal(vec2 N) const {
	uint32_t iMin(0);
	float dMin(-FLT_MAX);

	for (uint32_t i = 0; i<4; i++) {
		float d = glm::dot(N, GetNormal(i));
		if (d > dMin) {
			dMin = d;
			iMin = i;
		}
	}

	return GetNormal(iMin);
}

// Pick the best support neighbor along n
// It's critical that this function return an array where the vertices
// are in the proper clockwise order
std::array<vec2, 2> OBB::GetSupportNeighbor(vec2 n, int idx) const {
	std::array<vec2, 2> ret;

	vec2 vb = GetVert(idx);
	vec2 va = GetVert(idx - 1);
	vec2 vc = GetVert(idx + 1);

	vec2 nab = glm::normalize(perp(vb - va));
	vec2 nbc = glm::normalize(perp(vc - vb));
	float d1 = glm::dot(nab, n);
	float d2 = glm::dot(nbc, n);

	if (d1 > d2)
		return{ { va, vb } };
	return{ { vb, vc } };
}

// Get verts, clockwise
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

// Get normal, clockwise
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

// Static function to project a point p along the edge
// between points e0 and e1
static vec2 projectOnEdge(vec2 p, vec2 e0, vec2 e1) {
	// u and v form a little triangle
	vec2 u = p - e0;
	vec2 v = e1 - e0;

	// find the projection of u onto v, parameterize it
	float t = glm::dot(u, v) / glm::dot(v, v);

	//std::cout << t << ", " << glm::dot(u, v) << ", " << glm::dot(v,v) << std::endl;

	// Clamp between two edge points
	return e0 + v * clamp(t, 0.f, 1.f);
}

// Pick the best feature pair (penetrating or separating); A is the "face" object, B is the "vertex" object
// Penetrating is a bit of a grey area atm
static void featurePairJudgement(FeaturePair& mS, FeaturePair& mP, OBB * A, OBB * B, FeaturePair::Type type) {
	// For all A's normals
	for (int fIdx = 0; fIdx < 4; fIdx++) {
		vec2 n = A->GetNormal(fIdx);
		vec2 p1 = A->GetVert(fIdx);
		vec2 p2 = A->GetVert((fIdx + 1) % 4);

		// For B's support verts relative to the normal
		std::array<int, 2> supportVerts = { { -1, -1 } };
		int nVerts = B->GetSupportIndices(-n, supportVerts);
		for (int s = 0; s < nVerts; s++) {
			int sIdx = supportVerts[s];
			vec2 sV = B->GetVert(sIdx);

			// minkowski face points
			vec2 mfp0 = sV - p1;
			vec2 mfp1 = sV - p2;

			// Find point on minkowski face
			vec2 p = projectOnEdge(vec2(), mfp0, mfp1);

			// are objects penetrating?
			// i.e is first mf point behind face normal
			// penetration implies support vert behind normal
			float dist = glm::dot(mfp0, n);
			float c_dist = glm::length(sV - A->C);
			bool isPenetrating = dist < 0.f;

			// fp points to the correct feature pair
			FeaturePair * fp(nullptr);
			if (isPenetrating)
				fp = &mP;
			else {
				fp = &mS;
				// Separation dist is the length from p to the origin
				dist = glm::length(p);
			}

			// See whether or not this new feature pair is a good candidate
			// For penetration, we want the largest negative value
			bool accept = false;
			if (isPenetrating ? (dist > fp->dist) : (dist < fp->dist))
				accept = true;
			else if (feq(dist, fp->dist)) {
				// If it's just about as close as the current best feature pair,
				// pick the one whose distance is closest to the center of the face object
				if (c_dist < fp->c_dist)
					accept = true;
			}

			// Reassign *fp as needed
			if (accept)
				*fp = FeaturePair(dist, c_dist, fIdx, sIdx, type);
		}
	}
}

// Closest points between two OBBs
std::list<Contact> OBB::GetClosestPoints(const OBB& other) const {
	std::list<Contact> ret;

	// TODO are penetrating features working?
	FeaturePair mostSeparated(FLT_MAX);
	FeaturePair mostPenetrating(-FLT_MAX);

	// Check our faces with their verts, then vice versa
	featurePairJudgement(mostSeparated, mostPenetrating, (OBB *)this, (OBB *)&other, FeaturePair::Type::F_V);
	featurePairJudgement(mostSeparated, mostPenetrating, (OBB *)&other, (OBB *)this, FeaturePair::Type::V_F);

	// Pick the correct feature pair
	bool sep = (mostSeparated.dist > 0 && mostSeparated.T != FeaturePair::Type::N);
	FeaturePair * fp = sep ? &mostSeparated : &mostPenetrating;

	// We better have something
	assert(fp->T != FeaturePair::Type::N);

	// A is face feature, B is vertex feature
	OBB * A(nullptr), *B(nullptr);
	if (fp->T == FeaturePair::Type::F_V) {
		A = (OBB *)this;
		B = (OBB *)&other;
	}
	else {
		B = (OBB *)this;
		A = (OBB *)&other;
	}

	// Get the world space normal and edge points
	// of the face feature
	vec2 wN = A->GetNormal(fp->fIdx);
	vec2 wE0 = A->GetVert(fp->fIdx);
	vec2 wE1 = A->GetVert(fp->fIdx + 1);

	// Get the world space vertex of the vertex feature,
	// and then get "supporting neighbor" of that vertex
	// along the direction of the face feature edge, clockwise
	std::array<vec2, 2> wV = B->GetSupportNeighbor(-wN, fp->vIdx);

	//std::cout << wN << "\n" << wE0 << "\n" << wE1 << "\n" << wV0 << "\n" << wV1 << "\n" << std::endl;

	// Project edge points along vertex feature edge
	vec2 p1 = projectOnEdge(wE0, wV[0], wV[1]);
	vec2 p2 = projectOnEdge(wE1, wV[0], wV[1]);

	// Project vertex points along face feature edge
	vec2 p3 = projectOnEdge(wV[0], wE0, wE1);
	vec2 p4 = projectOnEdge(wV[1], wE0, wE1);

	// distance is point distance along face (contact) normal
	float d1 = glm::dot(p1 - p4, wN);
	float d2 = glm::dot(p2 - p3, wN);

	// If they're equal, collapse the two into one contact
	// This could be used as an early out, if you have the balls
	if (feq(d1, d2)) {
		vec2 pA = 0.5f * (p1 + p2);
		vec2 pB = 0.5f * (p3 + p4);
		float d = glm::distance(pA, pB);
		ret.emplace_back(A, B, pA, pB, wN, d);
	}
	else
	{   // Otherwise add two contacts points
		ret.emplace_back(A, B, p1, p4, wN, d1);
		ret.emplace_back(A, B, p2, p3, wN, d2);
	}

	return ret;
}

// Not sure about this
std::list<Contact> OBB::GetClosestPoints(const Circle& other) const {
	vec2 a_pos = ws_clamp(other.C);
	vec2 n = -glm::normalize(a_pos - other.C);
	vec2 b_pos = other.C - n*other.r;
	float dist = glm::length(a_pos - b_pos);
	Contact c((RigidBody_2D *)this, (RigidBody_2D *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

// Definitely not the cheapest way of doing this
vec2 OBB::ws_clamp(vec2 p) const {
	vec2 p1 = glm::inverse(getRotMat()) * (p - C);
	p1 = glm::clamp(p1, -R, R);
	return C + getRotMat() * p1;
}

// Right now there are some inconsistencies in the way this function behaves
// compared to the OBB-OBB version. I think this one is correct, but it's tough to tell...
std::list<Contact> OBB::GetClosestPoints(const AABB& other) const {
	std::list<Contact> ret;

	// Find the vector from our center to theirs
	vec2 centerVecN = other.C - C;
	vec2 faceN(0);
	// Make it a unit vector in the direction of largest magnitude
	faceN = glm::normalize(maxComp(centerVecN));

	// Get supporting vertex / vertices along that direction
	std::array<int, 2> sV;
	int nSupportVerts = GetSupportIndices(faceN, sV);

	// one support vert means a vertex-face collision
	if (nSupportVerts == 1) {
		// Two contact points; the support vertex, and it's best neighbor
		std::array<glm::vec2, 2> pA_arr = GetSupportNeighbor(faceN, sV[0]);
		for (auto& pA : pA_arr) {
			vec2 pB = other.clamp(pA);
			float d = glm::distance(pA, pB);
			ret.emplace_back((RigidBody_2D *)this, (RigidBody_2D *)&other, pA, pB, faceN, d);
		}
	}
	// face-face collsion, average two support verts into one contact point
	else {
		vec2 pA = 0.5f * (GetVert(sV[0]) + GetVert(sV[1]));
		vec2 pB = other.clamp(pA);
		float d = glm::distance(pA, pB);
		ret.emplace_back((RigidBody_2D *)this, (RigidBody_2D *)&other, pA, pB, faceN, d);
	}

	return ret;
}

// NYI
bool OBB::IsOverlapping(const Circle& other) const {
	return false;
}

bool OBB::IsOverlapping(const AABB& other) const {
	return false;
}

// Overridden because AABB is special
void OBB::ChangeVel(vec2 newV, vec2 rad) {
	return RigidBody_2D::ChangeVel(newV, rad);
}