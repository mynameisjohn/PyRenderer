#include "Drawable.h"
#include "IqmFile.h"

#include <gtx/transform.hpp>

#include <array>

GLint Drawable::s_PosHandle(-1);

Drawable::Drawable() :
	m_VAO(0),
	m_nIdx(0),
	m_Color(1),
	m_MV(1)
{}

// This will probably have to be a bit more flexible
Drawable::Drawable(std::string& iqmFileName, vec4& color, mat4& MV) :
	m_SrcFile(iqmFileName),
	m_VAO(0),
	m_nIdx(0),
	m_Color(color),
	m_MV(MV)
{
	// Lambda to generate a VBO
	auto makeVBO = []
		(GLuint buf, GLint handle, void * ptr, GLsizeiptr numBytes, GLuint dim, GLuint type) {
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, numBytes, ptr, GL_STATIC_DRAW);
		glEnableVertexAttribArray(handle);
		glVertexAttribPointer(handle, dim, type, 0, 0, 0);
		//Disable?
	};

	IqmFile f(MODEL_DIR + m_SrcFile);

	std::array<GLuint, 2> vboBuf{ 0 };
	
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(vboBuf.size(), vboBuf.data());

	GLuint bufIdx(0);
	auto pos = f.Positions();
	makeVBO(vboBuf[bufIdx++], s_PosHandle, pos.ptr(), pos.numBytes(), pos.nativeSize() / sizeof(float), GL_FLOAT);

	auto idx = f.Indices();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBuf[bufIdx]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numBytes(), idx.ptr(), GL_STATIC_DRAW);

	m_nIdx = idx.count();
	
	glBindVertexArray(0);
}

mat4 Drawable::GetMV() const {
	return m_MV;
}

vec4 Drawable::GetColor() const {
	return m_Color;
}

void Drawable::LeftMultMV(mat4 M) {
	m_MV = M * m_MV;
}

void Drawable::Translate(vec3 T) {
	LeftMultMV(glm::translate(T));
}

void Drawable::Rotate(fquat Q) {
	LeftMultMV(glm::mat4_cast(Q));
}

void Drawable::Draw() {
	// Bind VAO, draw
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_nIdx, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(m_VAO);
}

/*static*/ void Drawable::SetPosHandle(GLint pH) {
	s_PosHandle = pH;
}

// Python overrides
bool Drawable::PyExpose(const std::string& name, PyObject * module) {
	Python::Expose_Object(this, name, module);
	return true;
}

// TODO
bool Drawable::PyUpdate() {

	return true;
}