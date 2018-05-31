#pragma once

#include <gamepad/joystick.h>
#include <libevdev-1.0/libevdev/libevdev.h>

namespace gamepad {

class LinuxJoystick : public Joystick {

private:
    struct libevdev* edev;

public:
    LinuxJoystick(struct libevdev* edev) {
        this->edev = edev;
    }

    virtual bool getButton(int index) const {
        return false;
    }

    virtual float getAxis(int index) const {
        return 0.f;
    }

    virtual int getHat(int index) const {
        return 0;
    }

};

}