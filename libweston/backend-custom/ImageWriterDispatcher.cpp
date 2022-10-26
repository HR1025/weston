#include "ImageWriterDispatcher.h"

#include <libweston/libweston.h>

namespace weston
{

void ImageWriterDispatcher::Write(const uint8_t* image, const uint32_t imageLen)
{
    for (auto& imageWriter : _imageWriterCollection)
    {
        imageWriter.second->Write(image, imageLen);
    }
}

void ImageWriterDispatcher::operator+=(std::shared_ptr<IImageWriter> imageWriter)
{
    if (_imageWriterCollection.count(imageWriter.get()) != 0)
    {
        weston_log("imageWriter is already registered!!!\n");
    }
    _imageWriterCollection[imageWriter.get()] = imageWriter;
}

void ImageWriterDispatcher::operator-=(std::shared_ptr<IImageWriter> imageWriter)
{
    if (_imageWriterCollection.count(imageWriter.get()) == 0)
    {
        weston_log("imageWriter is not registered!!!\n");
    }
    else
    {
        _imageWriterCollection.erase(imageWriter.get());
    }

}

} // namespace weston