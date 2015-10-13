#include <glm/glm.hpp>

#include "RigidBody.h"
#include "SpeculativeContacts.h"

// Smol
const float kEPS = 0.001f;

RigidBody_2D::RigidBody_2D() :
	V(0),
	C(0),
	m(1),
	e(1),
	th(0),
	w(0)
{}


RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e, float th) :
	V(V),
	C(C),
	m(m),
	e(e),
	th(th),
	w(0)
{}

// I guess this just advances the object?
void RigidBody_2D::Integrate() {
//	const float dT = 0.005f; // TODO better integration methods?
	vec2 delta = globalTimeStep * V;
	C += delta;
	th += w * globalTimeStep;

	// I used to have python do this, but what's the point?
	m_pEntity->HandleMessage(int(Entity::CompID::DRAWABLE),
		int(Entity::MsgID::DR_TRANSLATE));
}

vec2 RigidBody_2D::GetMomentum() const {
	return m * V;
}

float RigidBody_2D::GetKineticEnergy() const {
	return 0.5f * m * glm::dot(V, V);
}

// I don't think this is physically accurate,
// doesn't really deal with angular momentum
void RigidBody_2D::ChangeVel(vec2 newV, vec2 rad) {
	vec2 delV = newV - V;
	V = newV;
	vec2 perp(-rad.y, rad.x);
	w += glm::dot(delV, perp);
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

Circle::Circle(vec2 V, vec2 C, float m, float e, float radius, float th) :
	r(radius),
	RigidBody_2D(V, C, m, e, th)
{}

AABB::AABB() :
	R(1),
	RigidBody_2D()
{}

AABB::AABB(vec2 vel, vec2 c, float mass, float elasticity, vec2 r) :
	RigidBody_2D(vel, c, mass, elasticity, 0.f),
	R(r)
{}

AABB::AABB(vec2 v, float mass, float el, float x, float y, float w, float h) :
	R(vec2(w, h) / 2.f),
	RigidBody_2D(v, vec2(x, y) + vec2(w, h) / 2.f, mass, el, 0.f)
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

glm::mat2 OBB::getRotMat() const {
	float c = cos(th);
	float s = sin(th); // glm wants transpose
	return glm::mat2(vec2(c, s), vec2(-s, c));
}

int OBB::GetSupportVerts(vec2 n, std::array<vec2, 2>& sV) const {
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
		if (d > dMin) {
			dMin = d;
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
		if (fabs(d - dMin) < kEPS) {
			// Take it too
			dMin = d;
			sV[num++] = i;
		}
	}


	return num;
}

vec2 OBB::GetSupportNeighbor(vec2 n, int idx) const {
	std::array<vec2, 2> ret;

    vec2 vb = GetVert(idx);
    vec2 va = GetVert(idx-1);
    vec2 vc = GetVert(idx+1);
    
	vec2 nab = glm::normalize(va - vb);
	vec2 nbc = glm::normalize(vc - vb);
	if (glm::dot(nab, n) > glm::dot(nbc, n))
        return va;
    return vc;
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
    float t = glm::dot(u, v) / glm::dot(v,v);

    // Clamp between two edge points
	return e0 + v * clamp(t, 0.f, 1.f);
}

static void featurePairJudgement(FeaturePair& mS, FeaturePair& mP, OBB * A, OBB * B, FeaturePair::Type type){
    // For all A's normals
    for (int fIdx = 0; fIdx < 4; fIdx++) {
        vec2 n = A->GetNormal(fIdx);
        vec2 p1 = A->GetVert(fIdx);
        vec2 p2 = A->GetVert((fIdx + 1) % 4);
        
        // For B's support verts relative to the normal
        std::array<int, 2> supportVerts = { {-1, -1} };
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
            if (isPenetrating) {
                // For penetrating features, we're interested in the
                // largest negative distance (meaning closest to 0)
                if (dist > mP.dist)
                    mP = FeaturePair(dist, c_dist, fIdx, sIdx, type);
            }
            else {
                // Reassign dist
                dist = glm::length(p);
                
                // We're interested in the closest separated distance
                if (dist < mS.dist)
                    mS = FeaturePair(dist, c_dist, fIdx, sIdx, type);
                else {
                    float del = fabs(dist - mS.dist);
                    // If they're very close, pick the one whose vertex
                    // is closest to the face object's center
                    if (del < kEPS) {
                        if (c_dist < mS.c_dist)
                            mS = FeaturePair(dist, c_dist, fIdx, sIdx, type);
                    }
                }
            }
        }
    }
}

std::list<Contact> OBB::GetClosestPoints(const OBB& other) const {
	

	std::list<Contact> ret;
	FeaturePair mostSeparated(FLT_MAX);
	FeaturePair mostPenetrating(-FLT_MAX);

	// Check our faces with their verts, then vice versa
	featurePairJudgement(mostSeparated, mostPenetrating, (OBB *)this, (OBB *)&other, FeaturePair::Type::F_V);
	featurePairJudgement(mostSeparated, mostPenetrating, (OBB *)&other, (OBB *)this, FeaturePair::Type::V_F);

	// Pick the correct feature pair
	bool pen = (mostSeparated.dist < 0);
	FeaturePair * fp = pen ? &mostPenetrating : &mostSeparated;

    // Function that generates the actual contacts
	auto makePoints = [fp, &ret](OBB * A, OBB * B) {
		std::array<vec2, 4> contactPoints;

        // Get the world space normal and edge points
        // of the face feature
		vec2 wN = A->GetNormal(fp->fIdx);
		vec2 wE0 = A->GetVert(fp->fIdx);
		vec2 wE1 = A->GetVert(fp->fIdx + 1);
        
        // Get the world space vertex of the vertex feature,
        // and then get "supporting neighbor" of that vertex
        // along the direction of the face feature edge
        vec2 wV0 = B->GetVert(fp->vIdx);
        vec2 wV1 = B->GetSupportNeighbor(wE1-wE0, fp->vIdx);

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
	vec2 n = -glm::normalize(a_pos - other.C);
	vec2 b_pos = other.C - n*other.r;
	float dist = glm::length(a_pos - b_pos);
	Contact c((Circle *)this, (Circle *)&other, a_pos, b_pos, n, dist);
	return{ c };
}

// Definitely not the cheapest way of doing this
vec2 OBB::ws_clamp(vec2 p) const {
	vec2 p1 = glm::inverse(getRotMat()) * (p - C);
	p1 = glm::clamp(p1, -R, R);
	return C + getRotMat() * p1;
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
    vec3 T(C, 0.f);
    fquat Q(cos(th/2.f), vec3(0.f, 0.f, sin(th/2.f)));
	return quatvec(T, Q);
}

void OBB::ChangeVel(vec2 newV, vec2 rad) {
	return RigidBody_2D::ChangeVel(newV, rad);
}

FeaturePair::FeaturePair(float d, float cd, int f, int v, Type t) :
dist(d),
c_dist(cd),
fIdx(f),
vIdx(v),
T(t)
{}