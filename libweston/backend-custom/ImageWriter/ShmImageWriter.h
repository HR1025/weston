/**
 * @file     ShmImageWriter.h
 * @author   haorui
 * @brief    ImageWrite 共享内存写入器
 * @version  0.1
 * @date     2022-10-25
 * 
 * @note     将图像数据写入共享内存, 实现 IPC 图像数据传输
 *           内存模型:
 *           +++++++++++++++++++++++++++++++++++++++
 *           + image payload   | isUpdated(1 byte) +
 *           +++++++++++++++++++++++++++++++++++++++
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <string>
#include <memory>

#include "../IImageWriter.h"

#include <Poco/NamedMutex.h>
#include <Poco/SharedMemory.h>

namespace weston
{

class ShmImageWriter : public IImageWriter
{
public:
    /**
     * @brief       构造函数
     * @param[in]   namedMutex       : 有名互斥锁
     * @param[in]   namedMemory      : 有名内存 (共享内存)
     * @param[in]   namedMemorySize  : 有名内存大小
     * @sa          参见 Poco 获取更多信息
     * @note        IPC 
     */
    ShmImageWriter(const std::string& namedMutex, const std::string& namedMemory, uint32_t namedMemorySize);
    ~ShmImageWriter() = default;
public:
    void Write(const uint8_t* image, const uint32_t imageLen) override;
private:
    std::shared_ptr<Poco::NamedMutex>      _namedMutex;
    std::shared_ptr<Poco::SharedMemory>    _sharedMemory;
    uint32_t                               _sharedMemorySize;
};

} // namespace weston