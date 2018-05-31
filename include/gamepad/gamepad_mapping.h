#pragma once

#include <string>
#include <vector>
#include "gamepad_ids.h"

namespace gamepad {

class Joystick;

class GamepadMapping {

public:
    struct MapFrom {
        enum class Type {
            BUTTON, AXIS, HAT
        };
        Type type;

        union {
            struct {
                int id;
            } button;
            struct {
                int id;
                float min, max;
            } axis;
            struct {
                int id;
                int mask;
            } hat;
        } d;
    };
    struct MapTo {
        enum class Type {
            BUTTON, AXIS
        };
        Type type;

        union {
            struct {
                GamepadButton id;
            } button;
            struct {
                GamepadAxis id;
                float min, max;
            } axis;
        } d;
    };
    struct Mapping {
        MapFrom from;
        MapTo to;
    };

    std::vector<Mapping> mappings;


    void parse(std::string const&);

};

}