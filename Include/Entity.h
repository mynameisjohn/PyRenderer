#pragma once

#include <pyliason.h>
#include "Util.h"
#include <vec4.hpp>

// Generic pythone exposed object (should this be in PyLiaison?)
class PyExposed {
public:
	virtual bool PyExpose(const std::string& name, PyObject * module) = 0;
};

class Entity : public PyExposed {
public:
	// Probably end up making a class out of this
	using MessageQueue = std::list<std::function<bool(void)>>;
	enum class CompID : int {
		DRAWABLE,
		COLLISION
	};
	enum class MsgID : int {
		DR_TRANSLATE,
		DR_COLOR
	};

	// Maybe not the best idea
	friend class Scene;
protected:
	int m_UniqueID;
	int m_ColID;
	int m_DrID;
	Scene * m_Scene;
	MessageQueue m_MessageQ;
	Python::Object m_PyModule;
public:
	Entity();
	Entity(int id, Scene * scnPtr, Python::Object module, int cId = -1, int drId = -1);
	
	Python::Object GetPyModule()  const;
	int GetID() const;
	
	// Message handling stuff
	template <typename D>
	bool PostMessage(int C, int M, D data);

	// Void (no data) case
	bool PostMessage(int C, int M);

	// Who knows what it'll be?
	//bool PostMessage(int C, int M, Python::Object data);

	void Update();

	// Python expose override
	bool PyExpose(const std::string& name, PyObject * module) override;
};