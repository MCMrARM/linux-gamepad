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
    std::string dirname = "/dev/input";

    inotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (inotify > 0) {
        watch = inotify_add_watch(inotify, dirname.data(), IN_CREATE | IN_ATTRIB | IN_DELETE);
    }
    regex.assign("^event[0-9]+$");
    DIR* dir = opendir(dirname.data());
    if (dir) {
        dirent* entry;
        while ((entry = readdir(dir))) {
            if (!std::regex_match(entry->d_name, regex)) {
                continue;
            }
            onDeviceAdded(dirname + "/" + std::string(entry->d_name, strnlen(entry->d_name, sizeof(entry->d_name))));
        }
        closedir(dir);
    }
}

void LinuxJoystickManager::poll() {
    if(inotify > 0) {
        char buffer[16384];
        for(ssize_t i = 0, length = read(inotify, buffer, sizeof(buffer)); i < length; ) {
            auto e = (const inotify_event*)(buffer + i);
            if(e->len <= 0 || (e->name + e->len) > (buffer + length)) {
                printf("inotify_event unexpected e->len %ld\n", (long)e->len);
                break;
            }
            i += sizeof(inotify_event) + e->len;
            if(e->len < 1) {
                printf("inotify_event skip empty name\n");
                continue;
            }
            std::string devPath(e->name, strnlen(e->name, e->len));
            if (!std::regex_match(devPath, regex)) {
                continue;
            }
            devPath = "/dev/input/" + devPath;
            if(e->mask & (IN_CREATE | IN_ATTRIB)) {
                onDeviceAdded(devPath);
            } else if(e->mask & IN_DELETE) {
                onDeviceRemoved(devPath);
            }
        }
    }

    for (auto const& js : joysticks)
        js->poll();
}

void LinuxJoystickManager::onDeviceAdded(const std::string& devPath) {
    for(auto it = joysticks.begin(); it != joysticks.end(); it++) {
        if(it->get()->getPath() == devPath) {
            return;
        }
    }
    int fd = open(devPath.data(), O_RDONLY | O_NONBLOCK);
    if(fd == -1) {
        printf("open error %i (%s)\n", (int)errno, devPath.data());
        return;
    }
    libevdev* edev = nullptr;
    int err = libevdev_new_from_fd(fd, &edev);
    if(err) {
        printf("libevdev_new_from_fd error %i (%s)\n", err, devPath.data());
        close(fd);
        return;
    }

    auto js = std::make_unique<LinuxJoystick>(this, devPath, edev);
    onJoystickConnected(js.get());
    joysticks.push_back(std::move(js));
}

void LinuxJoystickManager::onDeviceRemoved(const std::string& devPath) {
    for(auto it = joysticks.begin(); it != joysticks.end(); it++) {
        if(it->get()->getPath() == devPath) {
            onJoystickDisconnected(it->get());
            joysticks.erase(it);
            return;
        }
    }
}

std::shared_ptr<JoystickManager> JoystickManagerFactory::create() {
    return std::make_shared<LinuxJoystickManager>();
}
