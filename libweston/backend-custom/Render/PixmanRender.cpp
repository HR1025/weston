#include "PixmanRender.h"

#include <cassert>
#include <libweston/libweston.h>

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef  __cplusplus
}
#endif

namespace weston
{

PixmanRender::PixmanRender()
{
    _opaqueOptions.insert(field::kCompositor);
    _opaqueOptions.insert(field::kOutput);
    _opaqueOptions.insert(field::kBuffer);
    _opaqueOptions.insert(field::kWidth);
    _opaqueOptions.insert(field::kHeight);
    _opaqueOptions.insert(field::kPixelFormat);

    _output      = nullptr;
    _compositor  = nullptr;
    _buffer      = nullptr;
    _image       = nullptr;
    _height      = -1;
    _width       = -1;
    _isEnabled   = false;
    _isInited    = false;
}

PixmanRender::~PixmanRender()
{

}

bool PixmanRender::Init()
{
    assert(_compositor);
    int ret = pixman_renderer_init(_compositor);
    if (ret < 0)
    {
        return false;
    }
    else
    {
        _isInited = true;
        return true;
    }
}

bool PixmanRender::Uinit()
{
    // nothing to do
    _isInited = true;
    return true;
}

bool PixmanRender::Enable()
{
    assert(_buffer);
    assert(_height != -1);
    assert(_width  != -1);
        
	const struct pixman_renderer_output_options options = {
		.use_shadow = true,
	};

    if (_isEnabled)
    {
        return true;
    }

    _image = pixman_image_create_bits(_format, _width, _height, _buffer, _width * 4);

    if (pixman_renderer_output_create(_output, &options) < 0)
    {
        goto err;
    }

    pixman_renderer_output_set_buffer(_output, _image);
    
    _isEnabled = true;

    return true;
err:
    pixman_image_unref(_image);

    return false;
} 

bool PixmanRender::Disable()
{
    assert(_compositor);
    assert(_image);

    if (!_isEnabled)
    {
        return true;
    }

    pixman_renderer_output_destroy(_output);
    pixman_image_unref(_image);
    
    _isEnabled = false;

    return true;
}

bool PixmanRender::Repaint(pixman_region32_t *damage)
{
    assert(_compositor);
    assert(_output);

    if (!_isEnabled)
    {
        return false;
    }

    _compositor->renderer->repaint_output(_output, damage);

    pixman_region32_subtract(&(_compositor->primary_plane.damage), &(_compositor->primary_plane.damage), damage);

    return true;
}

void PixmanRender::SetOpaqueData(std::string key, void* opaque)
{
    if (_opaqueOptions.count(key) == 0)
    {
        weston_log("PixmanRender : unknown key [%s]\n", key.c_str());
    }
    if (key == field::kCompositor)
    {
        _compositor = reinterpret_cast<weston_compositor*>(opaque);
    }
    else if (key == field::kOutput)
    {
        _output = reinterpret_cast<weston_output *>(opaque);
    }
    else if (key == field::kBuffer)
    {
        _buffer = reinterpret_cast<uint32_t*>(opaque);
    }
    else if (key == field::kWidth)
    {
        _width  = *reinterpret_cast<int32_t *>(opaque);
    }
    else if (key == field::kHeight)
    {
        _height = *reinterpret_cast<int32_t *>(opaque);
    }
    else if (key == field::kPixelFormat)
    {
        _format = *reinterpret_cast<pixman_format_code_t*>(opaque);
    }
}

void const* PixmanRender::GetOpaqueData(std::string key)
{
    if (_opaqueOptions.count(key) == 0)
    {
        weston_log("PixmanRender : unknown key [%s]\n", key.c_str());
        return nullptr;
    }
    if (key == field::kCompositor)
    {
        return reinterpret_cast<void*>(&_compositor);
    }
    else if (key == field::kOutput)
    {
        return reinterpret_cast<void*>(&_output);
    }
    else if (key == field::kBuffer)
    {
        return reinterpret_cast<void*>(&_buffer);
    }
    else if (key == field::kWidth)
    {
        return reinterpret_cast<void*>(&_width);
    }
    else if (key == field::kHeight)
    {
        return reinterpret_cast<void*>(&_height);
    }
    else if (key == field::kPixelFormat)
    {
        return reinterpret_cast<void*>(&_format);
    }

    return nullptr;
}

std::set<std::string /* key */> PixmanRender::GetOpaqueOptions()
{
    return _opaqueOptions;
}

} // namespace weston
