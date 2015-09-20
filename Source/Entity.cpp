#include "Entity.h"
#include "Drawable.h"
#include "RigidBody.h"

Entity::Entity(int id, Circle * cCmp, Drawable * drCmp) :
	m_UniqueID(id),
	m_ColCmp(cCmp),
	m_DrCmp(drCmp)
{}

bool Entity::PyExpose(const std::string& name, PyObject * module) {
	Python::Expose_Object(this, name, module);
	return true;
}

bool Entity::PostMessage(int C, int M) {
	bool handled = false;

	switch (C) {
	case int(CompID::DRAWABLE) : // These casts are unfortunate
		switch (M) {
		case int(MsgID::DR_TRANSLATE) : {
			const auto drPtr = m_DrCmp;
			const vec2 lastT = m_ColCmp->lastT;
			// Get last translation, apply, reset
			m_MessageQ.emplace_back([drPtr, lastT]() {
				drPtr->Translate(vec3(lastT, 0.f));
				return true;
			});
			//	std::bind(&Drawable::Translate, m_DrCmp, m_ColCmp->lastT));
			m_ColCmp->lastT = vec2(0.f);
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
			const auto drPtr = m_DrCmp;
			// Get last translation, apply, reset
			m_MessageQ.emplace_back([drPtr, v]() {
				drPtr->SetColor(v);
				return true;
			});
			//	std::bind(&Drawable::Translate, m_DrCmp, m_ColCmp->lastT));
			m_ColCmp->lastT = vec2(0.f);
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

// I'm putting this here, but I don't know why
PyComponent::PyComponent(Entity * ePtr) :
	m_pEntity(ePtr)
{}

Entity * PyComponent::GetEntity()  const {
	return m_pEntity;
}