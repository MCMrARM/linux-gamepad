#pragma once

#include <gamepad/joystick_manager.h>
#include <memory>
#include <regex>
#include <string>
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

    void onDeviceAdded(const std::string& devPath);
    void onDeviceRemoved(const std::string& devPath);

};

}