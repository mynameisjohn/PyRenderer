#include "Drawable.h"
#include "IqmFile.h"

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

	IqmFile f(m_SrcFile);

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

void Drawable::Draw() {

}

/*static*/ void Drawable::SetPosHandle(GLint pH) {
	s_PosHandle = pH;
}