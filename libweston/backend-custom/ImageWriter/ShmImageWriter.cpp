#include "ShmImageWriter.h"

#include <memory.h>

namespace weston
{

ShmImageWriter::ShmImageWriter(const std::string& namedMutex, const std::string& namedMemory, uint32_t namedMemorySize)
{
    _sharedMemorySize =  namedMemorySize;
    _namedMutex       =  std::make_shared<Poco::NamedMutex>(namedMutex);
    _sharedMemory     =  std::make_shared<Poco::SharedMemory>(namedMemory, namedMemorySize + 1, Poco::SharedMemory::AccessMode::AM_WRITE);
}

void ShmImageWriter::Write(const uint8_t* image, const uint32_t imageLen)
{
    _namedMutex->lock();
    uint32_t writeSize = _sharedMemorySize < imageLen ? _sharedMemorySize :  imageLen;
    memcpy(_sharedMemory->begin(), image, imageLen);
    _sharedMemory->begin()[_sharedMemorySize] = 1; // 1 for isUpdated
    _namedMutex->unlock();
}

} // namespace weston