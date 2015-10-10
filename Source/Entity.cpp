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

bool Entity::HandleMessage(int C, int M) {
	bool handled = false;

	switch ((CompID)C) {
	case CompID::DRAWABLE: // These casts are unfortunate
		switch ((MsgID)M) {
		case MsgID::DR_TRANSLATE: {
			Drawable * const drPtr = m_DrCmp;
			const quatvec qv = m_ColCmp->GetQuatVec();
			// Get last translation, apply, reset
			m_MessageQ.emplace_back([drPtr, qv]() {
				drPtr->SetTransform(qv);
				return true;
			});
			handled = 0;
		}
								  break;
		}
	}

	return handled;
}

bool Entity::HandleRequest(int C, int M, Python::Object data) {
	bool handled = false;

	switch (C) {
	case int(CompID::DRAWABLE) : // These casts are unfortunate
		switch (M) {
		case int(MsgID::DR_COLOR) : {
			// Unpack object as tuple<vec4>
			std::tuple<vec4> tup;
			if (data.convert(tup)) {
				vec4 v = std::get<0>(tup);
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

Entity * OwnedByEnt::GetEntity() const {
	return m_pEntity;
}

uint32_t OwnedByEnt::GetEntID() const {
	return 0;
}

void OwnedByEnt::SetEntity(Entity * const pEnt) {
	m_pEntity = pEnt;
}

void Entity::SetColCmp(RigidBody_2D * colPtr) {
	m_ColCmp = colPtr;
}

void Entity::SetDrCmp(Drawable * drPtr) {
	m_DrCmp = drPtr;
}