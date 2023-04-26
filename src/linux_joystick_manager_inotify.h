#pragma once

#include <gamepad/joystick_manager.h>
#include <memory>
#include <regex>
#include "linux_joystick.h"

namespace gamepad {

class LinuxJoystickManager : public JoystickManager {

private:
    std::vector<std::unique_ptr<LinuxJoystick>> joysticks;
    std::regex regex;
    int inotify = 0;
    int watch = 0;
public:
    LinuxJoystickManager() {}
    ~LinuxJoystickManager();

    void initialize() override;

    void poll() override;

    void onDeviceAdded(const char* devPath);
    void onDeviceRemoved(const char* devPath);

};

}