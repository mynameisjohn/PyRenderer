#include "Camera.h"
#include <gtx/transform.hpp>

GLint Camera::s_ProjHandle(-1);
GLint Camera::s_PosHandle(-1);
using glm::normalize;

Camera::Camera() :
	m_Type(Type::NIL),
	m_v3Pos(0),
	m_qRot(1, 0, 0, 0),
	m_m4Proj(1)
{}

Camera::Camera(glm::vec2 X, glm::vec2 Y, glm::vec2 Z) :
	m_Type(Type::ORTHO),
	m_v3Pos(0),
	m_qRot(1, 0, 0, 0),
	m_m4Proj(glm::ortho(X[0], X[1], Y[0], Y[1], Z[0], Z[1]))
{}

Camera::Camera(float fovy, float aspect, glm::vec2 nf) :
	m_Type(Type::PERSP),
	m_v3Pos(0, 0, 0),
	m_qRot(1, 0, 0, 0),
	m_m4Proj(glm::perspective(fovy, aspect, nf[0], nf[1]))
{}

int Camera::InitOrtho(float l, float r, float t, float b, float n, float f) {
	*this = Camera({ l,r }, { t,b }, { n, f });
	return 0;
}

void Camera::InitPersp(float fovy, float aspect, float n, float f) {
	*this = Camera(fovy, aspect, { n,f });
}

vec3 Camera::GetPos() {
	return m_v3Pos;
}

fquat Camera::GetRot() {
	return m_qRot;
}

vec3 Camera::getView() {
	return vec3(m_m4Proj*vec4(0, 0, 1, 1));
}

// Why am I inverting the translation but not the rotation?
mat4 Camera::GetTransform() {
	return glm::mat4_cast((m_qRot))*glm::translate(-m_v3Pos);
}

mat4 Camera::GetMat() {
	// The camera is always at the origin.
	// We're really moving and rotating everyone else
	return m_m4Proj*GetTransform();
}

void Camera::Rotate(fquat Q) {
	m_qRot = glm::normalize(Q * m_qRot);
}

void Camera::Translate(vec3 T) {
	// I honestly have no clue why this works
	vec3 Tp(glm::mat4_cast(glm::inverse(m_qRot))*vec4(T, 0));
	m_v3Pos += Tp;
}

void Camera::ResetRot() {
	m_qRot = { 1, 0, 0, 0 };
}

void Camera::ResetPos() {
	m_v3Pos = { 0, 0, 0 };
}

void Camera::Reset() {
	ResetPos();
	ResetRot();
}

mat4 Camera::GetProj() {
	return m_m4Proj;
}

/*static*/ GLint Camera::GetPosHandle() {
	return s_PosHandle;
}

/*static*/ GLint Camera::GetProjHandle() {
	return s_ProjHandle;
}

/*static*/ void Camera::SetPosHandle(GLint P) {
	s_PosHandle = P;
}

/*static*/ void Camera::SetProjHandle(GLint p) {
	s_ProjHandle = p;
}