#include "Entity.h"
#include "Drawable.h"
#include "RigidBody.h"
#include "Scene.h"

Entity::Entity() :
	m_UniqueID(-1),
	m_ColCmp(nullptr),
	m_DrCmp(nullptr),
	m_Scene(nullptr)
{}

Entity::Entity(int id, Scene * scnPtr, Python::Object module) :
	m_UniqueID(id),
	m_ColCmp(nullptr),
	m_DrCmp(nullptr),
	m_Scene(scnPtr),
	m_PyModule(module)
{}

bool Entity::PostMessage(int C, int M) {
	bool handled = false;

	switch (C) {
	case int(CompID::DRAWABLE) : // These casts are unfortunate
		switch (M) {
		case int(MsgID::DR_TRANSLATE) : {
			Drawable * const drPtr = m_DrCmp;
			const vec2 pos = m_ColCmp->C;
			// Get last translation, apply, reset
			m_MessageQ.emplace_back([drPtr, pos]() {
				drPtr->SetPos(vec3(pos, 0.f));
				return true;
			});
			handled = 0;
		}
										break;
		}
	}

	return handled;
}

// Vec4 specialization
template<>
bool Entity::PostMessage<vec4>(int C, int M, vec4 v) {
	bool handled = false;

	switch (C) {
	case int(CompID::DRAWABLE) : // These casts are unfortunate
		switch (M) {
		case int(MsgID::DR_COLOR) : {
			Drawable * const drPtr = m_DrCmp;
			// Get last translation, apply, reset
			m_MessageQ.emplace_back([drPtr, v]() {
				drPtr->SetColor(v);
				return true;
			});
			handled = 0;
		}
		}
	}

	return handled;
}

// Just run all posted messages
void Entity::Update() {
	for (auto& msg : m_MessageQ)
		msg();
	m_MessageQ.clear();
}

Python::Object Entity::GetPyModule()  const {
	return m_PyModule;
}

int Entity::GetID() const {
	return m_UniqueID;
}

OwnedByEnt::OwnedByEnt() :
	m_pEntity(nullptr)
{}

OwnedByEnt::OwnedByEnt(Entity * pEnt) :
	m_pEntity(pEnt)
{}