/**
 * @file     InputDevice.h
 * @author   haorui
 * @brief    封装 device input 的细节实现
 * @version  0.1
 * @date     2022-10-24     
 * 
 * @note     由于 weston 中的 input 处理略微复杂,代码迁移工程量较大,目前实际上 InputDevice 还是紧密
 *           依赖于 weston 的 input 实现 (libinput-seat.h、libinput-device.h etc),
 *           实际上 InputDevice 的初衷是希望其能够独立运行,而不依赖于现有的 weston 框架
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <functional>

// forward declaration
struct udev;
struct udev_monitor;

namespace weston
{

class InputDevice
{
public:
    using CreateInputHook   = std::function<bool(udev* _udev)>;
    using DestroyInputHook  = std::function<bool()>;
public:
    InputDevice();
    ~InputDevice();
public:
    /**
     * @brief       设置创建udev输入钩子
     * @param[in]   hook
     * @sa          udev_input_init
     */
    void SetCreateInputHook(const CreateInputHook& hook);
    /**
     * @brief       设置销毁udev输入钩子
     * @param[in]   hook
     * @sa          udev_input_destroy
     */
    void SetDestroyInputHook(const DestroyInputHook& hook);
public:
    /**
     * @brief     初始化
     * @return    成功返回 true
     */
    bool Init();
    /**
     * @brief     重置
     * @return    成功返回 true
     */
    bool UnIint();
private:
    bool                   _isInited;
    udev*                  _udev;
    udev_monitor*          _udevMonitor;
    CreateInputHook        _createInputHook;
    DestroyInputHook       _destroyInputHook;
};

} // namespace weston
