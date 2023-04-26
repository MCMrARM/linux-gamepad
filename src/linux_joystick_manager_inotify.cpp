// This is an altered inotify implementation from GLFW3
//========================================================================
// GLFW 3.4 Linux - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2017 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
// It is fine to use C99 in this file because it will not be built with VS
//========================================================================

#include "linux_joystick_manager_inotify.h"
#include <gamepad/joystick_manager_factory.h>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <dirent.h>

using namespace gamepad;

LinuxJoystickManager::~LinuxJoystickManager() {
}

// Copy from https://github.com/glfw/glfw/blob/3fa2360720eeba1964df3c0ecf4b5df8648a8e52/src/linux_joystick.c#L265
void LinuxJoystickManager::initialize() {
    const char* dirname = "/dev/input";

    inotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (inotify > 0)
    {
        // HACK: Register for IN_ATTRIB to get notified when udev is done
        //       This works well in practice but the true way is libudev

        watch = inotify_add_watch(inotify,
                                              dirname,
                                              IN_CREATE | IN_ATTRIB | IN_DELETE);
    }

    // Continue without device connection notifications if inotify fails

    regex.assign("^event[0-9]+$");

    DIR* dir = opendir(dirname);
    if (dir)
    {
        struct dirent* entry;

        while ((entry = readdir(dir)))
        {
            if (!std::regex_match(entry->d_name, regex))
                continue;

            char path[PATH_MAX];

            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

            onDeviceAdded(path);
        }

        closedir(dir);
    }
}

void LinuxJoystickManager::poll() {
    if (inotify > 0) {

        ssize_t offset = 0;
        char buffer[16384];
        const ssize_t size = read(inotify, buffer, sizeof(buffer));

        while (size > offset)
        {
            const struct inotify_event* e = (struct inotify_event*) (buffer + offset);

            offset += sizeof(struct inotify_event) + e->len;

            if (!std::regex_match(e->name, regex))
                continue;

            char path[PATH_MAX];
            snprintf(path, sizeof(path), "/dev/input/%s", e->name);

            if (e->mask & (IN_CREATE | IN_ATTRIB))
                onDeviceAdded(path);
            else if (e->mask & IN_DELETE)
            {
                onDeviceRemoved(path);
            }
        }
    }

    for (auto const& js : joysticks)
        js->poll();
}

void LinuxJoystickManager::onDeviceAdded(const char* devPath) {
    for (auto it = joysticks.begin(); it != joysticks.end(); it++) {
        if (strcmp(it->get()->getPath().c_str(), devPath) == 0) {
            return;
        }
    }
    int fd = open(devPath, O_RDONLY | O_NONBLOCK);
    struct libevdev* edev = nullptr;
    int err = libevdev_new_from_fd(fd, &edev);
    if (err) {
        printf("libevdev_new_from_fd error %i (%s)\n", err, devPath);
        close(fd);
        return;
    }

    std::unique_ptr<LinuxJoystick> js (new LinuxJoystick(this, devPath, edev));
    onJoystickConnected(js.get());
    joysticks.push_back(std::move(js));
}

void LinuxJoystickManager::onDeviceRemoved(const char* devPath) {
    if (devPath == nullptr)
        return;
    for (auto it = joysticks.begin(); it != joysticks.end(); it++) {
        if (strcmp(it->get()->getPath().c_str(), devPath) == 0) {
            onJoystickDisconnected(it->get());
            joysticks.erase(it);
            return;
        }
    }
}

std::shared_ptr<JoystickManager> JoystickManagerFactory::create() {
    return std::shared_ptr<JoystickManager>(new LinuxJoystickManager());
}
