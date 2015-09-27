#pragma once

#include "GL_Includes.h"
#include "Util.h"
#include "Entity.h"
#include "quatvec.h"

#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/quaternion.hpp>

class Drawable : public OwnedByEnt {
	std::string m_SrcFile;
	GLuint m_VAO;
	GLuint m_nIdx;
	vec4 m_Color;
	quatvec m_QV;
	float m_Scale; // Could be a part of quatvec...
	int m_EntID;
public:
	Drawable();
	Drawable(std::string iqmSrc, vec4 clr, vec3 pos, int ID, float scale = 1.f, fquat rot = fquat(0, 0, 0, 0));

	mat4 GetMV() const;
	vec4 GetColor() const;
	
	void Draw();

	void SetPos(vec3 T);
	void Translate(vec3 T);

	void SetRot(fquat Q);
	void Rotate(fquat Q);

	void SetColor(vec4 C);

	uint32_t GetEntID() const;

	void IntroduceToEnt() override;

	friend class Scene;
protected:
	static GLint s_PosHandle;
	static void SetPosHandle(GLint);

private:
	// Static VAO cache (string to VAO/nIdx)
	static std::map<std::string, std::array<GLuint, 2> > s_VAOCache;
};