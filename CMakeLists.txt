cmake_minimum_required (VERSION 2.6)

project(linux-gamepad LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

option(LINUX_GAMEPAD_INOTIFY "Use inotify instead of libudev to detect gamepads even if it has been found" OFF)

if(NOT LINUX_GAMEPAD_INOTIFY)
    find_package(Udev)
endif()
find_package(Evdev REQUIRED)

add_library(linux-gamepad include/gamepad/gamepad_manager.h include/gamepad/gamepad.h include/gamepad/gamepad_ids.h include/gamepad/gamepad_mapping.h include/gamepad/joystick.h src/gamepad.cpp src/gamepad_mapping.cpp src/gamepad_manager.cpp include/gamepad/joystick_manager.h include/gamepad/callback_list.h include/gamepad/joystick_manager_factory.h src/linux_joystick.cpp src/linux_joystick.h)
target_include_directories(linux-gamepad PUBLIC include/ ${EVDEV_INCLUDE_DIRS})
target_link_libraries(linux-gamepad ${EVDEV_LIBRARIES})
if(UDEV_FOUND AND (NOT LINUX_GAMEPAD_INOTIFY))
    target_sources(linux-gamepad PRIVATE src/linux_joystick_manager.cpp src/linux_joystick_manager.h)
    target_include_directories(linux-gamepad PUBLIC ${UDEV_INCLUDE_DIRS})
    target_link_libraries(linux-gamepad ${UDEV_LIBRARIES})
else()
    target_sources(linux-gamepad PRIVATE src/linux_joystick_manager_inotify.cpp src/linux_joystick_manager_inotify.h)
endif()