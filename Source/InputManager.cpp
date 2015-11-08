#include "InputManager.h"

#include <SDL_events.h>

int keyCode(SDL_KeyboardEvent * ke){
    return ke->keysym.sym;
}

InputManager::InputManager():
    m_ScreenDim(1),
    m_MousePos(0),
    m_DeltaMouse(0)
{}

KeyState::KeyState(SDL_KeyboardEvent * ke /* = nullptr */):
    repeat(ke ? ke->repeat : false),
    tme(Time::now())
{
}

void InputManager::setKeyState(SDL_KeyboardEvent * ke){
    auto it = m_KeyState.find(keyCode(ke));
    if (it == m_KeyState.end())
        m_KeyState[keyCode(ke)] = KeyState(ke);
    else
        it->second.repeat = ke->repeat;
}

void InputManager::clearKeyState(SDL_KeyboardEvent *ke){
    int k = keyCode(ke);
    auto it = m_KeyState.find(k);
    if (it != m_KeyState.end())
        m_KeyState.erase(it);
}

void InputManager::setMousePos(int x, int y){
    vec2 oldPos = m_MousePos;
    m_MousePos = vec2(x, y);
    m_DeltaMouse = m_MousePos - oldPos;
}

vec2 InputManager::GetScreenDim() const{
    return m_ScreenDim;
}

vec2 InputManager::GetMousePos() const{
    return m_MousePos;
}

vec2 InputManager::GetDeltaMouse() const{
    return m_DeltaMouse;
}

bool InputManager::IsKeyDown(int k) const{
    return (m_KeyState.find(k) != m_KeyState.end());
}

KeyState InputManager::GetKeyState(int k) const{
    auto it = m_KeyState.find(k);
    if (it != m_KeyState.end())
        return it->second;
    return KeyState();
}

std::map<int, KeyState> InputManager::GetKeys() const{
    return m_KeyState;
}

// Don't deal with repeat for now
void InputManager::HandleEvent(SDL_Event * e){
    switch (e->type) {
        case SDL_KEYDOWN:
            setKeyState(&e->key);
            break;
        case SDL_KEYUP:
            clearKeyState(&e->key);
            break;
        case SDL_MOUSEMOTION:
            setMousePos(e->motion.x, e->motion.y);
            break;
        // Mouse Button?
    }
}

// More o dis
#include <pyliason.h>

namespace Python
{
//    bool convert(PyObject * o, KeyState& v){
//        
//    }
    // TODO look into PyStructSequence
    PyObject * alloc_pyobject(const KeyState& v){
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;

        float keyTime = timeAsFloat(v.tme);
        
        PyObject * ksLst = PyList_New(2);
        PyList_SetItem(ksLst, 0, alloc_pyobject(v.repeat));
        PyList_SetItem(ksLst, 1, alloc_pyobject(keyTime));
        return ksLst;
    }
}