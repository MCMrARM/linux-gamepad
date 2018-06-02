#include <gamepad/gamepad_manager.h>
#include <gamepad/joystick_manager.h>
#include <gamepad/gamepad.h>
#include <gamepad/joystick.h>
#include <gamepad/gamepad_mapping.h>

using namespace gamepad;

GamepadManager::GamepadManager(JoystickManager& jsManager) : jsManager(jsManager) {
    using namespace std::placeholders;
    jsManager.onJoystickConnected.add(*this, std::bind(&GamepadManager::onJoystickConnected, this, _1));
    jsManager.onJoystickDisconnected.add(*this, std::bind(&GamepadManager::onJoystickDisconnected, this, _1));
    jsManager.onJoystickButton.add(*this, std::bind(&GamepadManager::onJoystickButton, this, _1, _2, _3));
    jsManager.onJoystickAxis.add(*this, std::bind(&GamepadManager::onJoystickAxis, this, _1, _2, _3));
}

void GamepadManager::addMapping(GamepadMapping& mapping) {
    mappings[mapping.guid] = mapping;
}

void GamepadManager::addMapping(std::string const& mapping) {
    GamepadMapping m;
    m.parse(mapping);
    addMapping(m);
}

int GamepadManager::takeGamepadId() {
    int p = takenGamepadLowId - 1;
    auto it = takenGamepadIds.lower_bound(takenGamepadLowId);
    while (it != takenGamepadIds.end() && *it == p + 1) {
        p = *it;
        it++;
    }
    if (p + 1 == takenGamepadLowId)
        takenGamepadLowId++;
    return p + 1;
}

void GamepadManager::putGamepadIdBack(int i) {
    if (i < takenGamepadLowId)
        takenGamepadLowId = i;
    takenGamepadIds.erase(i);
}

void GamepadManager::onJoystickConnected(Joystick* js) {
    int gpi = takeGamepadId();
    Gamepad* gp = new Gamepad(gpi, *js, getMapping(js));
    js->setGamepad(gp);
    onGamepadConnected(gp);
}

void GamepadManager::onJoystickDisconnected(Joystick* js) {
    Gamepad* gp = js->getGamepad();
    onGamepadDisconnected(gp);
    putGamepadIdBack(gp->getIndex());
    delete gp;
    js->setGamepad(nullptr);
}

GamepadMapping& GamepadManager::getMapping(Joystick* js) {
    std::string guid = js->getGUID();
    auto mapping = mappings.find(guid);
    if (mapping != mappings.end())
        return mapping->second;
    return defaultMapping;
}

void GamepadManager::onJoystickButton(Joystick* js, int button, bool state) {
    Gamepad* gp = js->getGamepad();
    if (gp == nullptr)
        return;
    for (auto const& map : gp->getMapping().mappings) {
        if (map.from.type != GamepadMapping::MapFrom::Type::BUTTON || map.from.d.button.id != button)
            continue;
        if (map.to.type == GamepadMapping::MapTo::Type::BUTTON) {
            onGamepadButton(gp, map.to.d.button.id, state);
        } else if (map.to.type == GamepadMapping::MapTo::Type::AXIS) {
            onGamepadAxis(gp, map.to.d.axis.id, state ? map.to.d.axis.max : map.to.d.axis.min);
        }
    }
}

void GamepadManager::onJoystickAxis(Joystick* js, int axis, float value) {
    Gamepad* gp = js->getGamepad();
    if (gp == nullptr)
        return;
    for (auto const& map : gp->getMapping().mappings) {
        if (map.from.type != GamepadMapping::MapFrom::Type::AXIS || map.from.d.axis.id != axis)
            continue;
        if (map.to.type == GamepadMapping::MapTo::Type::BUTTON) {
            if (map.from.d.axis.min < map.from.d.axis.max)
                onGamepadButton(gp, map.to.d.button.id, value >= (map.from.d.axis.min + map.from.d.axis.max) / 2);
            else
                onGamepadButton(gp, map.to.d.button.id, value <= (map.from.d.axis.min + map.from.d.axis.max) / 2);
        } else if (map.to.type == GamepadMapping::MapTo::Type::AXIS) {
            auto& a = map.from.d.axis;
            auto& d = map.to.d.axis;
            if (value < std::min(a.min, a.max) || value > std::max(a.min, a.max))
                continue;
            value = (value - a.min) / (a.max - a.min);
            value = d.min + value * (d.max - d.min);
            onGamepadAxis(gp, map.to.d.axis.id, value);
        }
    }
}