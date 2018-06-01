#pragma once

#include <gamepad/joystick.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <vector>
#include <bitset>

namespace gamepad {

class LinuxJoystickManager;

class LinuxJoystick : public Joystick {

private:
    LinuxJoystickManager* mgr;
    struct libevdev* edev;

    int buttons[KEY_CNT];
    static constexpr int BUTTON_COUNT = KEY_CNT;
    std::bitset<BUTTON_COUNT> buttonValues;

public:
    LinuxJoystick(LinuxJoystickManager* mgr, struct libevdev* edev);

    void poll();

    virtual bool getButton(int index) const {
        if (index < 0 || index >= BUTTON_COUNT)
            return false;
        return buttonValues[index];
    }

    virtual float getAxis(int index) const {
        return 0.f;
    }

    virtual int getHat(int index) const {
        return 0;
    }

};

}