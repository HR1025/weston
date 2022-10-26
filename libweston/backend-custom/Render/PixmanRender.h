#pragma once

#include "../IRender.h"

struct weston_compositor;
struct weston_output;

namespace weston
{

class PixmanRender : public IRender
{
public:
    PixmanRender();
    ~PixmanRender();
public:
    bool Init()      override;
    bool Uinit()     override;
    bool Enable()    override;
    bool Disable()   override;
    bool Repaint(pixman_region32_t *damage) override;
public:
    void SetOpaqueData(std::string key, void* opaque) override;
    std::set<std::string /* key */> GetOpaqueOptions() override;
    void const* GetOpaqueData(std::string key) override;
private: /* property set by SetOpaqueData */
    weston_compositor*        _compositor;
    weston_output*            _output;
    uint32_t*                 _buffer;       // Hint : _buffer 的生命周期并不由 PixmanRender 控制
    int32_t                   _height;       
    int32_t                   _width;
    pixman_format_code_t      _format;
private:
    pixman_image_t*    _image;  
    bool               _isEnabled;
    bool               _isInited;
private:
    std::set<std::string /* key */> _opaqueOptions;
};

} // namespace weston