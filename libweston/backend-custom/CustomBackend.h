/**
 * @file     CustomBackend.h
 * @author   haorui
 * @brief    封装各种组件 (render, compositor, output),实现完成的 backend 功能
 * @version  0.1
 * @date     2022-10-21     
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

// forward declaration
struct weston_compositor;
struct weston_backend;
struct weston_windowed_output_api;
struct weston_output;
struct weston_mode;


#include <string>
#include <memory>

#include "CustomDefine.h"

// forward declaration
struct udev_input;

namespace weston
{

// forward declaration
class IRender;
class InputDevice;
class ImageWriterDispatcher;

class CustomBackend
{
public:
    static CustomBackend* Instance();
    CustomBackend(CustomBackend& customBackend) = delete;
    CustomBackend(CustomBackend&& customBackend) = delete;
    CustomBackend operator=(CustomBackend& customBackend) = delete;
    CustomBackend operator=(CustomBackend&& customBackend) = delete;
public:
    CustomBackend();
    ~CustomBackend();
public: /* set operation */
    void SetRenderType(RenderResourceType renderType);
    /**
     * @brief     设置 compositor
     * @param[in] compositor
     * @note      只能在初始化之前设置一次
     */
    void SetCompositor(weston_compositor* compositor);
    /**
     * @brief 获取 compositor
     */
    weston_compositor* GetCompositor();
    /**
     * @brief      设置名称
     */
    void SetName(const std::string& name);
    /**
     * @brief      设置输出配置
     */
    void SetOutputConfig(const OutputConfig& outputConfig);
public: /* get operation */
    /**
     * @brief 获取 weston backend
     */
    weston_backend* GetBackend();
    /**
     * @brief 获取 weston output
     */
    weston_output*  GetOuput();
    /**
     * @brief      获取名称
     */
    const std::string& GetName();
    /**
     * @brief    获取输出配置
     */
    const OutputConfig& GetOutputConfig();
public:
    /**
     * @brief 初始化
     */
    bool Init();
    /**
     * @brief 重置
     */
    bool Uninit();
public: 
    /**
     * @brief  初始化 backend
     */
    void InitBackend();
    /**
     * @brief  重置 backend
     */
    void UninitBackend();
    /**
     * @brief  初始化 input device
     */
    bool InitInputDevice();
    /**
     * @brief   重置 input device
     */
    bool UninitInputDevice();
public: /* hook for weston (see custom_wrapper.cpp for more information) */
    /**
     * @brief 初始化输出配置
     */
    void InitOuputConfig();
    /**
     * @brief 初始化 output
     */
    void InitOutput();
    /**
     * @brief   启动 output
     * @return  true 代表成功
     */
    bool EnableOuput();
    /**
     * @brief       重置 output
     * @param[in]   release : 是否要释放实际的资源
     */
    void UninitOuput(bool release = false);
    /**
     * @brief    重置 compositor
     */
    void UnitCompositor();
    /**
     * @brief 更新定时器
     */
    void UpdateTimer();
    /**
     * @brief      重绘
     * @param[in]  output
     * @param[in]  damage : 损坏数据,即需要更新的数据
     */
    void Repaint(weston_output* output, pixman_region32_t* damage);
    /**
     * @brief      写入图像数据
     */
    void WriteImage();
    /**
     * @brief      开始重绘循环
     */
    void StartRepaintLoop();
private:
    /**
     * @brief    初始化 render
     * @return   true 代表成功
     */
    bool InitRender();
    /**
     * @brief    重置 render
     * @return   true 代表成功
     */
    bool UninitRender();
    /**
     * @brief 初始化 Image Writer
     */
    void InitImageWriter();
    /**
     * @brief  重置 Image Writer
     */
    void UninitImageWriter();
    /**
     * @brief 初始化 weston_windowed_output_api
     */
    void InitOuputApi();
    /**
     * @brief  重置 weston_windowed_output_api
     */
    void UninitOuputApi();        
private: /* weston relevant items */        
    weston_compositor*                           _compositor;
    weston_backend*                              _backend;      
    weston_output*                               _output;
    wl_event_source*                             _finish_frame_timer;
    weston_windowed_output_api*                  _api;
    weston_mode*                                 _mode;
private: /* status items */        
    bool                                         _isInited;
    bool                                         _isRenderInited;
    bool                                         _isOutputInited;
    bool                                         _isInputInited;
private: /* config items */        
    RenderResourceType                           _renderType;
    std::string                                  _name;
    OutputConfig                                 _outputConfig;        
private: /* render relevant */        
    uint32_t*                                    _imageBuf;
    uint32_t                                     _imageBufLen;
    std::shared_ptr<IRender>                     _render;
private: /* input device relevant */        
    std::shared_ptr<InputDevice>                 _inputDevice;
    udev_input*                                  _udevInput;
private: /* image writer relevant */
    std::shared_ptr<ImageWriterDispatcher>       _imageWriterDispatcher;
};

} // namespace weston