#include "Drawable.h"
#include "IqmFile.h"

#include <glm/gtx/transform.hpp>

#include <array>

GLint Drawable::s_PosHandle(-1);
std::map<std::string, std::array<GLuint, 2> > Drawable::s_VAOCache;

Drawable::Drawable() :
	m_VAO(0),
	m_nIdx(0),
	m_Color(1),
	m_Scale(0)
{}

// This will probably have to be a bit more flexible
Drawable::Drawable(std::string iqmFileName, vec4 color, quatvec qv, float scale) :
	m_SrcFile(iqmFileName),
	m_VAO(0),
	m_nIdx(0),
	m_Color(color),
	m_QV(qv),
	m_Scale(scale)
{
	// Get rid of the .iqm extension (for no real reason) (and it better be there)
	m_SrcFile = m_SrcFile.substr(0, m_SrcFile.find(".iqm"));

	// See if we've loaded this Iqm File before
	if (s_VAOCache.find(m_SrcFile) == s_VAOCache.end()) {
		GLuint VAO(0), nIdx(0);

		// Lambda to generate a VBO
		auto makeVBO = []
			(GLuint buf, GLint handle, void * ptr, GLsizeiptr numBytes, GLuint dim, GLuint type) {
			glBindBuffer(GL_ARRAY_BUFFER, buf);
			glBufferData(GL_ARRAY_BUFFER, numBytes, ptr, GL_STATIC_DRAW);
			glEnableVertexAttribArray(handle);
			glVertexAttribPointer(handle, dim, type, 0, 0, 0);
			//Disable?
		};

		IqmFile f(MODEL_DIR + iqmFileName);

		std::array<GLuint, 2> vboBuf{ 0 };

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(vboBuf.size(), vboBuf.data());

		GLuint bufIdx(0);
		auto pos = f.Positions();
		makeVBO(vboBuf[bufIdx++], s_PosHandle, pos.ptr(), pos.numBytes(), pos.nativeSize() / sizeof(float), GL_FLOAT);

		auto idx = f.Indices();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBuf[bufIdx]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.numBytes(), idx.ptr(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		nIdx = idx.count();

		s_VAOCache[m_SrcFile] = { VAO, nIdx };
	}
	
	m_VAO = s_VAOCache[m_SrcFile][0];
	m_nIdx = s_VAOCache[m_SrcFile][1];
}

mat4 Drawable::GetMV() const {
	return m_QV.ToMat4() * glm::scale(vec3(m_Scale));
}

vec4 Drawable::GetColor() const {
	return m_Color;
}

// Todo correct these by type
void Drawable::Translate(vec3 T) {
	m_QV.V += T;
}

void Drawable::SetPos(vec3 T) {
	m_QV.V = T;
}

void Drawable::Rotate(fquat Q) {
	m_QV.Q = Q * m_QV.Q;
}

void Drawable::SetRot(fquat Q) {
	m_QV.Q = Q;
}

void Drawable::SetTransform(quatvec QV) {
	m_QV = QV;
}

void Drawable::Transform(quatvec QV) {
	// TODO
	m_QV.V += QV.V;
	m_QV.Q = QV.Q*m_QV.Q;
}

void Drawable::SetColor(vec4 C) {
	m_Color = glm::clamp(C, vec4(0), vec4(1));
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