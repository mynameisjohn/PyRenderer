#pragma once

#include <pyliason.h>
#include "Util.h"
#include <vec4.hpp>

class OwnedByEnt {
protected:
	Entity * m_pEntity;
public:
	OwnedByEnt();
	OwnedByEnt(Entity * pEnt);
	Entity * GetEntity();
	uint32_t GetEntID() const;
};

class Entity {
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

private:
	int m_UniqueID;
	RigidBody_2D * m_ColCmp;
	Drawable * m_DrCmp;
	Scene * m_Scene;
	MessageQueue m_MessageQ;
	Python::Object m_PyModule;


protected:
	friend class Scene;
	void SetColCmp(RigidBody_2D * colPtr);
	void SetDrCmp(Drawable * drPtr);

public:
	Entity();
	Entity(int id, Scene * scnPtr, Python::Object module);
	
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
};