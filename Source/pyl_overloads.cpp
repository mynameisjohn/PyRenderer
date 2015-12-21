#include "pyl_overloads.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

#include "Util.h"
#include "InputManager.h"

namespace Python
{
	//    bool convert(PyObject * o, KeyState& v){
	//        
	//    }
	// TODO look into PyStructSequence
	PyObject * alloc_pyobject(const KeyState& v) {
		using std::chrono::duration_cast;
		using std::chrono::milliseconds;

		float keyTime = timeAsFloat(v.tme);

		PyObject * ksLst = PyList_New(2);
		PyList_SetItem(ksLst, 0, alloc_pyobject(v.repeat));
		PyList_SetItem(ksLst, 1, alloc_pyobject(keyTime));
		return ksLst;
	}
}