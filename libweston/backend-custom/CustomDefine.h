/**
 * @file     CustomDefine.h
 * @author   haorui
 * @brief    通用定义
 * @version  0.1
 * @date     2022-10-21     
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <cstdint>

#ifdef  __cplusplus
extern "C" {
#endif

#include <libweston/pixman-renderer.h>

#ifdef  __cplusplus
}
#endif

namespace weston
{

namespace field
{

// Render opaque key
constexpr char kCompositor[]                 =  "compositor";         // weston_compositor *
constexpr char kOutput[]                     =  "output";             // weston_output *
constexpr char kBuffer[]                     =  "buffer";             // uint32_t *
constexpr char kWidth[]                      =  "width";              // int32_t *
constexpr char kHeight[]                     =  "height";             // int32_t *
constexpr char kPixelFormat[]                =  "pixelFormat";        // pixman_format_code_t *

// InputDevice
constexpr char kUdev[]                       =  "udev";
constexpr char kSeat[]                       =  "seat0";

// ImageWriter
// Hint : ffplay -f rawvideo -pixel_format rgb32 -video_size 1920x1080 -framerate 60 weston.dat (modify according to your profile)
constexpr char kDumpFilePath[]               =  "./weston.dat";
constexpr char kNamedMutex[]                 =  "weston_mutex";
constexpr char kNamedMemory[]                =  "weston_memory";

} // namespace field

/**
 * @brief 渲染资源类型
 */
enum class RenderResourceType
{
    INVALID,
    CPU,
    GPU,
};

/**
 * @brief 渲染器类型
 * 
 */
enum class RenderType
{
    PIXMAN = 0,
    OPENGL,      // no support for now
    NOOP,        // no support for now    
};

/**
 * @brief 输出配置
 */
struct OutputConfig
{
    uint32_t                    height;    // 高
    uint32_t                    width;     // 宽
    uint32_t                    refresh;   // 刷新率(Hz)
    pixman_format_code_t        format;    // 像素格式
};

} // namespace weston
