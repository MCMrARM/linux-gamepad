#pragma once

#include <functional>
#include "callback_list.h"

namespace gamepad {

class Joystick;

class JoystickManager {

public:
    CallbackList<std::function<void (Joystick* js)>> onJoystickConnected;
    CallbackList<std::function<void (Joystick* js)>> onJoystickDisconnected;

    virtual void poll() = 0;

};

}