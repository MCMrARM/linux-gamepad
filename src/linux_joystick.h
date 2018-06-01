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

    struct AxisInfo {
        int index;
        int min, max;
        int flat, fuzz;
    };

    int buttons[KEY_CNT];
    AxisInfo axis[ABS_CNT];

    static constexpr int BUTTON_COUNT = KEY_CNT;
    std::bitset<BUTTON_COUNT> buttonValues;
    static constexpr int AXIS_COUNT = ABS_CNT;
    float axisValues[AXIS_COUNT];
    static constexpr int HAT_COUNT = (ABS_HAT3Y + 1 - ABS_HAT0X) / 2;
    int hatValues[HAT_COUNT];

    inline bool isHat(int index) {
        return (index >= ABS_HAT0X && index <= ABS_HAT3Y);
    }

public:
    LinuxJoystick(LinuxJoystickManager* mgr, struct libevdev* edev);

    void poll();

    virtual bool getButton(int index) const {
        if (index < 0 || index >= BUTTON_COUNT)
            return false;
        return buttonValues[index];
    }

    virtual float getAxis(int index) const {
        if (index < 0 || index >= AXIS_COUNT)
            return false;
        return axisValues[index];
    }

    virtual int getHat(int index) const {
        if (index < 0 || index >= HAT_COUNT)
            return false;
        return hatValues[index];
    }

};

}