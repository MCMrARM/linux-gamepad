#pragma once

#include "gamepad_ids.h"

namespace gamepad {

class Joystick;
class GamepadMapping;

class Gamepad {

private:
    Joystick& joystick;
    GamepadMapping& mapping;

public:
    Gamepad(Joystick& joystick, GamepadMapping& mapping) : joystick(joystick), mapping(mapping) {}

    Joystick const& getJoystick() const {
        return joystick;
    }

    int getIndex() const;

    bool getButton(GamepadButton index) const;

    float getAxis(GamepadAxis index) const;

};

}