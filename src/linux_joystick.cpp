#include "linux_joystick.h"
#include "linux_joystick_manager.h"
#include <cerrno>
#include <cstdio>
#include <cstring>

using namespace gamepad;

#define LBITS (sizeof(long) * 8)
#define NLONGS(x) (((x) - 1) / LBITS + 1)
#define TEST_BIT(v, i) !!((v)[(i) / LBITS] & (1L << ((i) % LBITS)))

LinuxJoystick::LinuxJoystick(LinuxJoystickManager* mgr, struct libevdev* edev) : mgr(mgr), edev(edev) {
    int fd = libevdev_get_fd(edev);

    memset(buttons, 0xff, sizeof(buttons)); // sets all the buttons to -1 by default
    // buttonValues is zero-initialized by the bitset
    memset(axisValues, 0, sizeof(axisValues));

    unsigned long keys[NLONGS(KEY_CNT)];
    int len = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys);
    if (len < 0)
        throw std::runtime_error("Failed to get joystick keys");
    len *= 8;

    int nextId = 0;
    // SDL first maps the buttons after BTN_JOYSTICK (BTN_JOYSTICK - BTN_MAX),
    // and then the ones before it (0 - BTN_JOYSTICK)
    for (size_t i = BTN_JOYSTICK; i < len + BTN_JOYSTICK; i++) {
        if (TEST_BIT(keys, i % len))
            buttons[i] = nextId++;
    }

    unsigned long abs[NLONGS(ABS_CNT)];
    len = ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs)), abs);
    if (len < 0)
        throw std::runtime_error("Failed to get joystick abs");
    len *= 8;

    nextId = 0;
    for (size_t i = 0; i < len; i++) {
        if (!TEST_BIT(abs, i)) {
            axis[i].index = -1;
            continue;
        }
        if (i == ABS_HAT0X) { // skip hats
            i = ABS_HAT3Y;
            continue;
        }
        const input_absinfo* absinfo = libevdev_get_abs_info(edev, i);
        if (absinfo == nullptr)
            continue;
        float mm = 1.f / (absinfo->minimum + absinfo->maximum);
        axis[i] = {nextId++, absinfo->minimum, absinfo->maximum, absinfo->flat * mm, absinfo->fuzz * mm};
    }
}

void LinuxJoystick::poll() {
    struct input_event e;
    while (true) {
        int r = libevdev_next_event(edev, LIBEVDEV_READ_FLAG_NORMAL, &e);
        if (r == -EAGAIN)
            break;
        if (r != 0) {
            printf("LinuxJoystick::pool error\n");
            break;
        }
        if (e.type == EV_KEY) {
            if (e.code >= KEY_CNT)
                continue;
            int btn = buttons[e.code];
            if (btn == -1)
                continue;
            bool v = e.value != 0;
            buttons[btn] = v;
            if (mgr)
                mgr->onJoystickButton(this, btn, v);
        } else if (e.type == EV_ABS) {
            if (e.code >= AXIS_COUNT)
                continue;
            auto& a = axis[e.code];
            if (a.index == -1)
                continue;
            float v;
            if (e.value >= 0)
                v = (float) e.value / a.max;
            else
                v = - (float) e.value / a.min;
            v = std::min(std::max(v, 0.f), 1.f);
            if (std::abs(v) < a.flat)
                v = 0.f;
            axisValues[a.index] = v;
            if (mgr)
                mgr->onJoystickAxis(this, a.index, v);
        }
    }
}