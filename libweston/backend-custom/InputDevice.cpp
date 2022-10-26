#include "InputDevice.h"

#include <cassert>

#include <libweston/libweston.h>
#include <libudev.h>

#include "CustomDefine.h"

namespace weston
{

InputDevice::InputDevice()
{
    _isInited           = false;
    _udev               = nullptr;
    _udevMonitor        = nullptr;
    _createInputHook    = [](udev* _udev) -> bool { return true; };
    _destroyInputHook   = []() -> bool { return true; };
}

InputDevice::~InputDevice()
{
    UnIint();

    assert(!_udev);
    assert(!_udevMonitor);
}

void InputDevice::SetCreateInputHook(const CreateInputHook& hook)
{
    _createInputHook = hook;
}

void InputDevice::SetDestroyInputHook(const DestroyInputHook& hook)
{
    _destroyInputHook = hook;
}

bool InputDevice::Init()
{
    if (_isInited)
    {
        return true;
    }

    _udev = udev_new();
    if (_udev == nullptr)
    {
        weston_log("udev_new fail.\n");
        goto err;
    }

    if (!_createInputHook(_udev))
    {
        weston_log("init input fail.\n");
        goto err1;
    }

    _udevMonitor = udev_monitor_new_from_netlink(_udev, field::kUdev);
    if (_udevMonitor == nullptr)
    {
        weston_log("udev_monitor_new_from_netlink fail.\n");
        goto err2;
    }

    if (udev_monitor_enable_receiving(_udevMonitor) < 0)
    {
        weston_log("udev_monitor_enable_receiving fail.\n");
        goto err3;
    }

    _isInited = true;
    return true;

err3:
    udev_monitor_unref(_udevMonitor);
    _udevMonitor = nullptr;
err2:
    _destroyInputHook();
err1:
    udev_unref(_udev);
    _udev = nullptr;
err:
    return false;
}

bool InputDevice::UnIint()
{
    if (!_isInited)
    {
        return true;
    }

    udev_monitor_unref(_udevMonitor);
    _udevMonitor = nullptr;
    _destroyInputHook();
    udev_unref(_udev);
    _udev = nullptr;

    _isInited = false;
    return true;
}


} // namespace weston