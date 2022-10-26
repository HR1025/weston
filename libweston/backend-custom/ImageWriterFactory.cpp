#include "ImageWriterFactory.h"

#include <libweston/libweston.h>

#include "ImageWriter/FileImageWriter.h"
#include "ImageWriter/DummyImageWriter.h"

#include "config.h"

#if BUILD_WITH_POCO
#include "ImageWriter/ShmImageWriter.h"
#endif /* BUILD_WITH_POCO */

namespace weston
{

std::shared_ptr<IImageWriter> ImageWriterFactory::CreateFileImageWriter(std::string filePath)
{
    return std::make_shared<FileImageWriter>(filePath);
}

std::shared_ptr<IImageWriter> ImageWriterFactory::CreateShmImageWriter(const std::string& namedMutex, const std::string& namedMemory, uint32_t namedMemorySize)
{
#if BUILD_WITH_POCO
    return std::make_shared<ShmImageWriter>(namedMutex, namedMemory, namedMemorySize);
#else
    weston_log("can not create ShmImageWriter, rebuild with poco!!!\n");
    return std::make_shared<DummyImageWriter>();
#endif /* BUILD_WITH_POCO */
}

} // namespace weston
