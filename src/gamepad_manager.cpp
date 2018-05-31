#include <gamepad/gamepad_manager.h>

using namespace gamepad;

void GamepadManager::addMapping(GamepadMapping& mapping) {
    mappings[mapping.guid] = mapping;
}

void GamepadManager::addMapping(std::string const& mapping) {
    GamepadMapping m;
    m.parse(mapping);
    addMapping(m);
}