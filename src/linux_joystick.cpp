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

    unsigned long keys[NLONGS(KEY_CNT)];
    int len = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys);
    if (len < 0)
        throw std::runtime_error("Failed to get joystick keys");
    len *= 8;

    int nextBtnId = 0;
    // SDL first maps the buttons after BTN_JOYSTICK (BTN_JOYSTICK - BTN_MAX),
    // and then the ones before it (0 - BTN_JOYSTICK)
    for (int i = BTN_JOYSTICK; i < len + BTN_JOYSTICK; i++) {
        if (TEST_BIT(keys, i % len))
            buttons[i] = nextBtnId++;
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
        }
    }
}