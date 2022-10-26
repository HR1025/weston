/**
 * @file     FileImageWriter.h
 * @author   haorui
 * @brief    ImageWrite 文件写入器
 * @version  0.1
 * @date     2022-10-25
 * 
 * @note     将图像数据写入文件 (目前主要用于排查渲染数据,以及在无输出的情况下调试功能),
 *           IO 访问会导致 Write 接口耗时,建议仅在无屏幕调试过程中打开 
 *           (1080P 下一帧 8M, 一秒 60 帧, 磁盘写入为 480 MB/s ...)
 *           TODO : FileImageWriter 采用拷贝的形式存储 Image, 另开线程与 Image 队列配合 (从 Sync 修改为 Async)
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <fstream>

#include "../IImageWriter.h"

namespace weston
{

class FileImageWriter : public IImageWriter
{
public:
    /**
     * @brief      构造函数
     * @param[in]  filePath : 文件路径
     * @note       覆盖打开,而非追加
     */
    explicit FileImageWriter(std::string filePath);
    ~FileImageWriter();
public:
    void Write(const uint8_t* image, const uint32_t imageLen) override;
private:
    std::ofstream  _ofs;
    std::string    _filePath;
    uint64_t       _imageNum;
    bool           _canWrite;
};

} // namespace weston