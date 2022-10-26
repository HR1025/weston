/**
 * @file     ImageWriterFactory.h
 * @author   haorui
 * @brief    ImageWrite 工厂
 * @version  0.1
 * @date     2022-10-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "IImageWriter.h"

#include <memory>

namespace weston
{

class ImageWriterFactory
{
public:
    /**
     * @brief       创建 FileImagerWriter
     * @param[in]   filePath : 写入文件路径
     * @note        目前 FileImageWriter 更多还是应用于调试
     */
    static std::shared_ptr<IImageWriter> CreateFileImageWriter(std::string filePath);
    /**
     * @brief       创建 ShmImageWriter
     * @param[in]   namedMutex       : 有名互斥锁
     * @param[in]   namedMemory      : 有名内存 (共享内存)
     * @param[in]   namedMemorySize  : 有名内存大小
     * @sa          参见 Poco 获取更多信息
     * @note        此接口依赖于 Poco, 若未存在 Poco 依赖,则返回 DummyImageWriter
     */
    static std::shared_ptr<IImageWriter> CreateShmImageWriter(const std::string& namedMutex, const std::string& namedMemory, uint32_t namedMemorySize);
};

} // namespace weston