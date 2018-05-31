#pragma once

namespace gamepad {

enum class GamepadButton {
    A, B, X, Y, LB, RB, BACK, START, GUIDE, LEFT_STICK, RIGHT_STICK,
    DPAD_UP, DPAD_RIGHT, DPAD_DOWN, DPAD_LEFT
};
enum class GamepadAxis {
    LEFT_X, LEFT_Y, RIGHT_X, RIGHT_Y, LEFT_TRIGGER, RIGHT_TRIGGER
};

}