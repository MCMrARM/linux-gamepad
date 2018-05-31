cmake_minimum_required (VERSION 2.6)

project(linux-gamepad LANGUAGES CXX)

add_library(linux-gamepad include/gamepad/gamepad_manager.h include/gamepad/gamepad.h include/gamepad/gamepad_ids.h include/gamepad/gamepad_mapping.h include/gamepad/joystick.h src/gamepad.cpp)
target_include_directories(linux-gamepad PUBLIC include/)