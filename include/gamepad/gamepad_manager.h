#pragma once

#include <functional>
#include <vector>
#include <set>
#include "gamepad_mapping.h"
#include "callback_list.h"

namespace gamepad {

class JoystickManager;
class Gamepad;

class GamepadManager : public CallbackAutoHandler {

public:
    GamepadManager(JoystickManager& jsManager);

    CallbackList<std::function<void (Gamepad* gp)>> onGamepadConnected;
    CallbackList<std::function<void (Gamepad* gp)>> onGamepadDisconnected;

    void addMapping(GamepadMapping& mapping);
    void addMapping(std::string const& mapping);

protected:
    JoystickManager& jsManager;
    GamepadMapping defaultMapping;
    std::unordered_map<std::string, GamepadMapping> mappings;
    std::set<int> takenGamepadIds;
    int takenGamepadLowId = 0;

    int takeGamepadId();
    void putGamepadIdBack(int i);

    GamepadMapping& getMapping(Joystick* js);

    void onJoystickConnected(Joystick* js);
    void onJoystickDisconnected(Joystick* js);

};

}