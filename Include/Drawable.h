#pragma once

#include "GL_Includes.h"
#include "Util.h"
#include "Entity.h"
#include "quatvec.h"

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class Drawable : public OwnedByEnt {
	std::string m_SrcFile;
	GLuint m_VAO;
	GLuint m_nIdx;
	vec4 m_Color;
	quatvec m_QV;
	vec2 m_Scale; // Could be a part of quatvec...
public:
	Drawable();
	Drawable(std::string iqmSrc, vec4 clr, quatvec qv, vec2 scale);

	mat4 GetMV() const;
	vec4 GetColor() const;
	
	void Draw();

	void SetPos(vec3 T);
	void Translate(vec3 T);

	void SetRot(fquat Q);
	void Rotate(fquat Q);

	void SetTransform(quatvec QV);
	void Transform(quatvec QV);

	void SetColor(vec4 C);

	friend class Scene;
protected:
	static GLint s_PosHandle;
	static void SetPosHandle(GLint);

private:
	// Static VAO cache (string to VAO/nIdx)
	static std::map<std::string, std::array<GLuint, 2> > s_VAOCache;
};