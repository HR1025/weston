/**
 * @file     IImageWriter.h
 * @author   haorui
 * @brief    图像写入器抽象
 * @version  0.1
 * @date     2022-10-24     
 * 
 * @note     backend-custom 并不像 backend-drm 或 backend-fbdev 管理具体的输出设备,故 backend-custom
 *           并不关心渲染数据(Image)用于何种目的, IImageWriter 提供了一个抽象的入口,允许用户通过继承其实现
 *           自己的业务逻辑
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <cstdint>

namespace weston
{

class IImageWriter
{
public:
    IImageWriter() = default;
    virtual ~IImageWriter() = default;
public:
    /**
     * @brief       写入图像数据
     * @param[in]   image    : 图像数据
     * @param[in]   imageLen : 图像数据长度
     */
    virtual void Write(const uint8_t* image, const uint32_t imageLen) = 0;
};

} // namespace weston