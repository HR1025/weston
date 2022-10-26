/**
 * @file     ImageWriterDispatcher.h
 * @author   haorui
 * @brief    图像抽象写入器
 * @version  0.1
 * @date     2022-10-24     
 * 
 * @note     IImageWriter 能够解决渲染数据流向何处的问题,但是 IImageWriter 更多的是指定渲染数据流向
 *           某一个具体的方向;在实际业务场景中,同一渲染数据在同一时间流向不同方向的需求是很常见的,
 *           ImageWriterDispatcher 完成这种渲染分发的过程
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <memory>
#include <unordered_map>

#include "IImageWriter.h"

namespace weston
{

class ImageWriterDispatcher : public IImageWriter
{
public:
    using ImageWriterCollection = std::unordered_map<void* /* address */, std::shared_ptr<IImageWriter>>;
public:
    void operator+=(std::shared_ptr<IImageWriter> imageWriter);
    void operator-=(std::shared_ptr<IImageWriter> imageWriter);
public:
    void Write(const uint8_t* image, const uint32_t imageLen) override;
private:
    ImageWriterCollection _imageWriterCollection;
};

} // namespace weston