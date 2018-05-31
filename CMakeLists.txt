cmake_minimum_required (VERSION 2.6)

project(linux-gamepad LANGUAGES CXX)

add_library(linux-gamepad include/gamepad/gamepad_manager.h include/gamepad/gamepad.h include/gamepad/gamepad_ids.h include/gamepad/gamepad_mapping.h include/gamepad/joystick.h src/gamepad.cpp src/gamepad_mapping.cpp src/gamepad_manager.cpp include/gamepad/joystick_manager.h include/gamepad/callback_list.h include/gamepad/joystick_manager_factory.h src/linux_joystick_manager.cpp src/linux_joystick_manager.h src/linux_joystick.cpp src/linux_joystick.h)
target_include_directories(linux-gamepad PUBLIC include/)
target_link_libraries(linux-gamepad udev evdev)