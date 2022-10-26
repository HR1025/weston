#include "FileImageWriter.h"

#include <libweston/libweston.h>

namespace weston
{

FileImageWriter::FileImageWriter(std::string filePath)
{
    _canWrite = false;
    _imageNum = 0;
    _filePath = filePath;

    try
    {
        _ofs.open(_filePath);
        if (_ofs.is_open())
        {
            _canWrite = true;
        }
    }
    catch(const std::exception& e)
    {
        weston_log("%s\n", e.what());
    }
    
}

FileImageWriter::~FileImageWriter()
{
    if (_ofs.is_open())
    {
        _ofs.close();
        weston_log("image write num is: %ld\n", _imageNum);
    }
}

void FileImageWriter::Write(const uint8_t* image, const uint32_t imageLen)
{
    if (_canWrite && _ofs.is_open())
    {
        _ofs.write(reinterpret_cast<const char*>(image), imageLen);
        _imageNum++;
    }
}

} // namespace weston
