/**
 * @file     DummyImageWriter.h
 * @author   haorui
 * @brief    ImageWrite 写入器空实现
 * @version  0.1
 * @date     2022-10-25
 * 
 * @note     主要用于实现编译级别的多态,比如一个 ImageWriter 的依赖不齐全,工厂将返回 DummyImageWriter 确保程序仍然可以运行
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "../IImageWriter.h"

namespace weston
{

class DummyImageWriter : public IImageWriter
{
    void Write(const uint8_t* image, const uint32_t imageLen) override
    {
        // nothing to
    }
};

} // namespace weston