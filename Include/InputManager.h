#pragma once

#include "Util.h"

#include <set>
#include <glm/vec2.hpp>

union SDL_Event;
struct SDL_KeyboardEvent;

//struct KeyState{
//    KeyState(SDL_KeyboardEvent * ke);
//    KeyState(int k);
//    bool repeat;
//    int key;
//    struct comp{
//        bool operator()(const KeyState& a, const KeyState& b){
//            return a.key < b.key;
//        }
//    };
//    
//    bool operator<(const KeyState& other){
//        return key < other.key;
//    }
//    bool operator==(const KeyState& other){
//        return key == other.key;
//    }
//};

struct KeyState{
    bool repeat;
    decltype(Time::now()) tme;
    KeyState(SDL_KeyboardEvent * ke = nullptr);
};

class InputManager{
public:
    InputManager();
    void HandleEvent(SDL_Event * e);
    bool IsKeyDown(int k) const;
    vec2 GetScreenDim() const;
    vec2 GetMousePos() const;
    vec2 GetDeltaMouse() const;
    KeyState GetKeyState(int k) const;
    std::map<int, KeyState> GetKeys() const;
    
private:
    std::map<int, KeyState> m_KeyState;
    
    vec2 m_ScreenDim;
    vec2 m_MousePos;
    vec2 m_DeltaMouse;
    
    void setKeyState(SDL_KeyboardEvent * ke);
    void clearKeyState(SDL_KeyboardEvent * ke);
    void setMousePos(int x, int y);
};