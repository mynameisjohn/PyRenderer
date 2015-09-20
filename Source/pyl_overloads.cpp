#include "pyl_overloads.h"

namespace Python
{
	bool convert(PyObject * o, vec2& v) {
		return convert_buf(o, &v[0], 2);
	}
	bool convert(PyObject * o, vec3& v) {
		return convert_buf(o, &v[0], 3);
	}
	bool convert(PyObject * o, vec4& v) {
		return convert_buf(o, &v[0], 4);
	}
	bool convert(PyObject * o, fquat& v) {
		return convert_buf(o, &v[0], 4);
	}
}