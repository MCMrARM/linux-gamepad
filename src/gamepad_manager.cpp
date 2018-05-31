#include <gamepad/gamepad_manager.h>
#include <gamepad/joystick_manager.h>
#include <gamepad/gamepad.h>
#include <gamepad/joystick.h>

using namespace gamepad;

GamepadManager::GamepadManager(JoystickManager& jsManager) : jsManager(jsManager) {
    using namespace std::placeholders;
    jsManager.onJoystickConnected.add(*this, std::bind(&GamepadManager::onJoystickConnected, this, std::placeholders::_1));
    jsManager.onJoystickDisconnected.add(*this, std::bind(&GamepadManager::onJoystickDisconnected, this, std::placeholders::_1));
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
    return defaultMapping;
}