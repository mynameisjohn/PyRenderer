#pragma once

#include "GL_Includes.h"
#include "Util.h"

#include <vec4.hpp>
#include <mat4x4.hpp>

class Drawable {
	std::string m_SrcFile;
	GLuint m_VAO;
	GLuint m_nIdx;
	vec4 m_Color;
	mat4 m_MV;
	
public:
	Drawable();
	mat4 GetMV() const;
	vec4 GetColor() const;

	Drawable(std::string&, vec4&, mat4&);
	void Draw();

	friend class Scene;
protected:
	static GLint s_PosHandle;
	static void SetPosHandle(GLint);
};