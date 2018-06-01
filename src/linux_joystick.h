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
        float flat, fuzz;
    };

    int buttons[KEY_CNT];
    AxisInfo axis[ABS_CNT];

    static constexpr int BUTTON_COUNT = KEY_CNT;
    std::bitset<BUTTON_COUNT> buttonValues;
    static constexpr int AXIS_COUNT = ABS_CNT;
    float axisValues[AXIS_COUNT];

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
        return 0;
    }

};

}