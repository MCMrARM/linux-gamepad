#pragma once

#include "gamepad_mapping.h"
#include "callback_list.h"

namespace gamepad {

class JoystickManager;

class GamepadManager : public CallbackAutoHandler {

public:
    GamepadManager(JoystickManager& joystickManager) : joystickManager(joystickManager) {}

    void addMapping(GamepadMapping& mapping);
    void addMapping(std::string const& mapping);

protected:
    JoystickManager& joystickManager;

    std::unordered_map<std::string, GamepadMapping> mappings;

};

}