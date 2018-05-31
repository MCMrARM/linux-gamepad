#pragma once

namespace gamepad {

class Gamepad;

class Joystick {

private:
    Gamepad* gamepad = nullptr;

public:
    virtual bool getButton(int index) const = 0;

    virtual float getAxis(int index) const = 0;

    virtual int getHat(int index) const = 0;


    Gamepad* getGamepad() const {
        return gamepad;
    }

    void setGamepad(Gamepad* gamepad) {
        this->gamepad = gamepad;
    }

};

}