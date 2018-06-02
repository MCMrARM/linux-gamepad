#include <gamepad/gamepad.h>
#include <gamepad/gamepad_mapping.h>
#include <gamepad/joystick.h>

using namespace gamepad;

bool Gamepad::getButton(GamepadButton index) const {
    for (auto const& map : mapping.mappings) {
        if (map.to.type != GamepadMapping::MapTo::Type::BUTTON || map.to.d.button.id != index)
            continue;

        if (map.from.type == GamepadMapping::MapFrom::Type::BUTTON) {
            if (joystick.getButton(map.from.d.button.id))
                return true;
        } else if (map.from.type == GamepadMapping::MapFrom::Type::AXIS) {
            auto& a = map.from.d.axis;
            float v = joystick.getAxis(a.id);
            if (map.from.d.axis.min < map.from.d.axis.max) {
                if (v >= (map.from.d.axis.min + map.from.d.axis.max) / 2)
                    return true;
            } else {
                if (v <= (map.from.d.axis.min + map.from.d.axis.max) / 2)
                    return true;
            }
        } else  if (map.from.type == GamepadMapping::MapFrom::Type::HAT) {
            int v = joystick.getHat(map.from.d.hat.id);
            if (v & map.from.d.hat.mask)
                return true;
        }
    }
    return false;
}

float Gamepad::getAxis(GamepadAxis index) const {
    for (auto const& map : mapping.mappings) {
        if (map.to.type != GamepadMapping::MapTo::Type::AXIS || map.to.d.axis.id != index)
            continue;

        if (map.from.type == GamepadMapping::MapFrom::Type::BUTTON) {
            if (joystick.getButton(map.from.d.button.id))
                return map.to.d.axis.max;
        } else if (map.from.type == GamepadMapping::MapFrom::Type::AXIS) {
            auto& a = map.from.d.axis;
            auto& d = map.to.d.axis;
            float v = joystick.getAxis(a.id);
            if (v < std::min(a.min, a.max) || v > std::max(a.min, a.max))
                continue;
            v = (v - a.min) / (a.max - a.min);
            return d.min + v * (d.max - d.min);
        } else  if (map.from.type == GamepadMapping::MapFrom::Type::HAT) {
            int v = joystick.getHat(map.from.d.hat.id);
            if (v & map.from.d.hat.mask)
                return map.to.d.axis.max;
        }
    }
    return 0.f;
}