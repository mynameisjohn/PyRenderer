#include "pyl_overloads.h"

#include <glm.hpp>
#include <gtc/quaternion.hpp>

namespace Python
{
	bool convert(PyObject * o, glm::vec2& v) {
		return convert_buf(o, &v[0], 2);
	}
	bool convert(PyObject * o, glm::vec3& v) {
		return convert_buf(o, &v[0], 3);
	}
	bool convert(PyObject * o, glm::vec4& v) {
		return convert_buf(o, &v[0], 4);
	}
	bool convert(PyObject * o, glm::fquat& v) {
		return convert_buf(o, &v[0], 4);
	}
}