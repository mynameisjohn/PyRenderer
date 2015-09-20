#include "RigidBody.h"

#include <glm.hpp>

RigidBody_2D::RigidBody_2D(vec2 V, vec2 C, float m, float e) :
	V(V),
	C(C),
	m(m),
	e(e),
	lastT(0)
{}

bool Circle::PyExpose(const std::string& name, PyObject * module) {
	Python::Expose_Object(this, name, module);

	return true;
}

bool Circle::IsColliding(Circle& other) {
	float dist = glm::length(C - other.C);
	float totalRadius = r + other.r;

	return (dist < totalRadius);
}

void Circle::ApplyCollision(Circle& other) {
	float Msum_1 = 1.f / (m+other.m); // denominator
	float Cr = 0.5f * (e+other.e); // coef of rest
	vec2 P1 = m * other.V, P2 = m * other.V;
	vec2 A = P1 + P2, B = Cr * (P1 - P2);
	vec2 v1 = (A - B) * Msum_1;
	vec2 v2 = (A + B) * Msum_1;
	V = v1;
	other.V = v2;

	// For now just hack it
	m_pEntity->GetPyModule().call_function("HandleCollision", m_pEntity->GetID());
}

// I guess this just advances the object?
void Circle::Update() {
	const float dT = 0.001f; // TODO better integration methods?
	vec2 delta = dT * V;
	lastT += delta; // Entity should reset this
	C += delta;
}

// TODO get this to post a translation message
bool Circle::PyUpdate() {
	//m_pEntity->GetPyModule().call_function
	return true;
}

Circle::Circle(vec2 V, vec2 C, float m, float e, float r, Entity * pEnt):
	RigidBody_2D(V,C,m,e), PyComponent(pEnt)
{
	this->r = r;
}