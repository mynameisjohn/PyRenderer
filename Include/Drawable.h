#pragma once

#include "GL_Includes.h"
#include "Util.h"
#include "Entity.h"

#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/quaternion.hpp>

class Drawable : public PyComponent {
	std::string m_SrcFile;
	GLuint m_VAO;
	GLuint m_nIdx;
	vec4 m_Color;
	mat4 m_MV;
	
public:
	Drawable();
	mat4 GetMV() const;
	vec4 GetColor() const;

	Drawable(std::string&, vec4&, mat4&, Entity *);
	void Draw();

	void LeftMultMV(mat4 M);
	void Translate(vec3 T);
	void Rotate(fquat Q);

	void SetColor(vec4 C);

	// Python overrides
	bool PyExpose(const std::string& name, PyObject * module) override;
	bool PyUpdate() override;

	friend class Scene;
protected:
	static GLint s_PosHandle;
	static void SetPosHandle(GLint);

private:
	// Static VAO cache (string to VAO/nIdx)
	static std::map<std::string, std::array<GLuint, 2> > s_VAOCache;
};