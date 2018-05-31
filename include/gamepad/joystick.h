#pragma once

class Joystick {

public:
    virtual int getIndex() const = 0;

    virtual bool getButton(int index) const = 0;

    virtual float getAxis(int index) const = 0;

    virtual int getHat(int index) const = 0;

};