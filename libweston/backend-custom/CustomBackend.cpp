#include "CustomBackend.h"

#include <string.h>
#include <cassert>

#include <wayland/wayland-server-core.h>
#include <libweston/libweston.h>
#include <wayland/wayland-util.h>


#ifdef  __cplusplus
extern "C" {
#endif
#include <libweston/backend.h>
#include <libweston/libweston-internal.h>
#include <libweston/linux-explicit-synchronization.h>
#include <libweston/plugin-registry.h>
#include <libweston/windowed-output-api.h>
#include <libweston/libinput-seat.h>
#include <libweston/launcher-util.h>
#ifdef  __cplusplus
}
#endif

#include "IRender.h"
#include "InputDevice.h"
#include "ImageWriterDispatcher.h"
#include "ImageWriterFactory.h"

#include "custom_wrapper.cpp"

namespace weston
{

CustomBackend* CustomBackend::Instance()
{
    static CustomBackend gCustomBackend;
    return &gCustomBackend;
}

CustomBackend::CustomBackend()
{
    _renderType           = RenderResourceType::INVALID;
    _compositor           = nullptr;
    _finish_frame_timer   = nullptr;
    _imageBuf             = nullptr;
    _backend              = new weston_backend();
    _api                  = new weston_windowed_output_api();
    _output               = new weston_output();
    _mode                 = new weston_mode();
    _udevInput            = new udev_input();
    _isInited             = false;
    _isRenderInited       = false;
    _isOutputInited       = false;
    _isInputInited        = false;
    _outputConfig         = {};
    _imageBufLen          = 0;

    _inputDevice          = std::make_shared<InputDevice>();
}

CustomBackend::~CustomBackend()
{
    delete    _backend;
    delete    _api;
    delete    _output;
    delete    _imageBuf;
    delete    _mode;
    delete    _udevInput;
}

void CustomBackend::SetRenderType(RenderResourceType renderType)
{
    _renderType = renderType;
}

void CustomBackend::SetCompositor(struct weston_compositor* compositor)
{
    assert(!_compositor);
    _compositor = compositor;
}

struct weston_backend* CustomBackend::GetBackend()
{
    assert(_backend);
    return _backend;
}

struct weston_compositor* CustomBackend::GetCompositor()
{
    assert(_compositor);
    return _compositor;
}


void CustomBackend::SetName(const std::string& name)
{
    _name = name;
}

void CustomBackend::SetOutputConfig(const OutputConfig& outputConfig)
{
    assert(_mode);
    _outputConfig   =  outputConfig;
}

const std::string& CustomBackend::GetName()
{
    return _name;
}

weston_output*  CustomBackend::GetOuput()
{
    assert(_output);
    assert(_isOutputInited);
    return _output;
}

bool CustomBackend::Init()
{
    assert(_compositor);
    if (_isInited)
    {
        return true;
    }
    InitOuputApi();
    InitBackend();
    InitImageWriter();
    // 使用软件定时器
    if (weston_compositor_set_presentation_clock_software(_compositor) < 0)
    {
        weston_log("weston_compositor_set_presentation_clock_software fail.");
        return false;
    }
    if (!InitRender())
    {
        weston_log("render init fail.");
        goto err;
    }
    if (linux_explicit_synchronization_setup(_compositor) < 0)
    {
        weston_log("linux_explicit_synchronization_setup fail.");
        goto err;
    }
    if (weston_plugin_api_register(_compositor, WESTON_WINDOWED_OUTPUT_API_NAME, _api, sizeof(weston_windowed_output_api)) < 0)
    {
        weston_log("weston_plugin_api_register fail");
        goto err;
    }

    _compositor->launcher = weston_launcher_connect(_compositor, 0, field::kSeat, true);
    if (!InitInputDevice())
    {
        weston_log("Init Input Device fail.\n");
        goto err;
    }
    
    _isInited = true;
    return true;
err:
    weston_compositor_shutdown(_compositor);
    return false;
}

bool CustomBackend::Uninit()
{
    UninitOuputApi();
    UninitBackend();
    UninitImageWriter();
    UninitRender();
    UninitInputDevice();
    _isInited = false;
    return true;
}

void CustomBackend::InitOuputApi()
{
    assert(_api);
    _api->create_head      = custom_head_create;
    _api->output_set_size  = custom_output_set_size;
}

void CustomBackend::UninitOuputApi()
{
    assert(_api);
    memset(_api, 0, sizeof(weston_windowed_output_api));
}

void CustomBackend::InitBackend()
{
    assert(_backend);
    assert(_compositor);
    _backend->create_output   = custom_backend_create_output;
    _backend->destroy         = custom_backend_destroy;
    _backend->repaint_begin   = custom_backend_repaint_begin;
    _backend->repaint_cancel  = custom_backend_repaint_cancel;
    _backend->repaint_flush   = custom_backend_repaint_flush;
    _compositor->backend     = _backend;
}

void CustomBackend::UninitBackend()
{
    assert(_backend);
    memset(_backend, 0, sizeof(weston_backend));
}

bool CustomBackend::InitInputDevice()
{
    assert(_compositor);
    assert(_udevInput);

    if (_isInputInited)
    {
        return true;
    }
    
    // set input hook
    // FIXME : bad solution, try to decouple these from weston, make InputDevice independent
    _inputDevice->SetCreateInputHook(
        [this](udev* _udev) -> bool
        {
            if (udev_input_init(this->_udevInput, this->_compositor, _udev, field::kSeat, nullptr /* for now */) < 0)
            {
                weston_log("udev_input_init fail.\n");
                return false;
            }
            return true;
        }
    );
    _inputDevice->SetDestroyInputHook(
        [this]() -> bool
        {
            udev_input_destroy(this->_udevInput);
            return true;
        }
    );

    if (!_inputDevice->Init())
    {
        return false;
    }

    _isInputInited = true;
    return true;
}

/**
 * @brief   重置 input device
 */
bool CustomBackend::UninitInputDevice()
{
    if (!_isInputInited)
    {
        return true;
    }

    if (!_inputDevice->UnIint())
    {
        weston_log("Uninit Input Device fail.\n");
        return false;
    }

    _isInputInited = true;
    return true;
}

void CustomBackend::InitOuputConfig()
{
    assert(_output);
    assert(_mode);

    // FIXME : the follow code is not surely necessary
    struct weston_head* head;
    wl_list_for_each(head, &(_output->head_list), output_link)
    {
        weston_head_set_monitor_strings(head, "weston", "custom",
                                        NULL);
        weston_head_set_physical_size(head, _outputConfig.width, _outputConfig.height);
    }
    _mode->flags    =  WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED;
    _mode->width    =  _outputConfig.width;
    _mode->height   =  _outputConfig.height;
    _mode->refresh  =  _outputConfig.refresh * 1000;
    wl_list_insert(&(_output->mode_list), &(_mode->link));
    _output->current_mode = _mode;
}

void CustomBackend::InitOutput()
{
    assert(_name.empty() != true);
    weston_output_init(_output, _compositor, _name.c_str());
    _output->destroy              =  custom_output_destroy;
    _output->disable              =  custom_output_disable;
    _output->enable               =  custom_output_enable;
    _output->attach_head          =  nullptr;
    _output->start_repaint_loop   =  custom_output_start_repaint_loop;
    _output->repaint              =  custom_output_repaint;
    weston_compositor_add_pending_output(_output, _compositor);
    _isOutputInited = true;
}

bool CustomBackend::EnableOuput()
{
    assert(_output);
    assert(_compositor);
    assert(_render);
    struct wl_event_loop* loop;
    loop = wl_display_get_event_loop(_compositor->wl_display);
    _finish_frame_timer = wl_event_loop_add_timer(loop, finish_frame_handler, _output);
    if (_finish_frame_timer == NULL)
    {
        weston_log("wl_event_loop_add_timer fail.\n");
        return false;
    }
    if (!_render->Enable())
    {
        weston_log("enable render fail.\n");
        return false;
    }
    return true;
}

void CustomBackend::UninitOuput(bool release)
{
    assert(_output);
    if (!_isOutputInited)
    {
        return;
    }
    wl_event_source_remove(_finish_frame_timer);
    assert(_render->Disable());
    if (release)
    {
        weston_output_release(_output);
    }
    _isOutputInited = false;
}

bool CustomBackend::InitRender()
{
    /**
     * @brief 设置 pixman opaque data
     * @note  需要在 pixman 任何操作前进行
     */
    static auto setPixmanRenderOpaqueData = [this](std::shared_ptr<IRender> render) -> void
    {
        render->SetOpaqueData(field::kBuffer, this->_imageBuf);
        render->SetOpaqueData(field::kCompositor, this->_compositor);
        render->SetOpaqueData(field::kHeight, &(this->_outputConfig.height));
        render->SetOpaqueData(field::kWidth, &(this->_outputConfig.width));
        render->SetOpaqueData(field::kPixelFormat, &(this->_outputConfig.format));
        render->SetOpaqueData(field::kOutput, this->_output);
    };
    assert(_compositor);
    assert(_output);
    bool res = false;
    if (RenderResourceType::INVALID == _renderType)
    {
        weston_log("must init render type before InitRender. exit directly!!!\n");
        abort();
    }
    if (RenderResourceType::GPU == _renderType)
    {
        weston_log("now not support use gpu to render. exit directly!!!\n");
        abort();
    }
    if (RenderResourceType::CPU == _renderType)
    {
        delete _imageBuf;
        _imageBuf    = reinterpret_cast<uint32_t*>(new char[_outputConfig.width * _outputConfig.height * 4]);
        _imageBufLen = _outputConfig.width * _outputConfig.height * 4;
        _render   = IRender::Create(RenderType::PIXMAN);
        setPixmanRenderOpaqueData(_render);
        res = _render->Init();
        if (res == true)
        {
            _isRenderInited = true;
        }
    }
    return res;
}

bool CustomBackend::UninitRender()
{
    _render.reset();
    _isRenderInited = false;
    return false;
}

void CustomBackend::InitImageWriter()
{
    _imageWriterDispatcher = std::make_shared<ImageWriterDispatcher>();
#if BUILD_BACKEND_CUSTOM_DUMP
    *(_imageWriterDispatcher.get()) += ImageWriterFactory::CreateFileImageWriter(field::kDumpFilePath);
#endif
    *(_imageWriterDispatcher.get()) += ImageWriterFactory::CreateShmImageWriter(field::kNamedMutex, field::kNamedMemory, _outputConfig.width * _outputConfig.height * 4);
}

void CustomBackend::UninitImageWriter()
{
    _imageWriterDispatcher.reset();
}


void CustomBackend::UnitCompositor()
{
    assert(_compositor);
    weston_head* base, *next;
    weston_compositor_shutdown(_compositor);
    wl_list_for_each_safe(base, next, &_compositor->head_list, compositor_link)
    weston_head_release(base);
}

void CustomBackend::UpdateTimer()
{
    assert(_finish_frame_timer);
    wl_event_source_timer_update(_finish_frame_timer, 1000 / _outputConfig.refresh);
}

void CustomBackend::Repaint(weston_output* output, pixman_region32_t* damage)
{
    assert(_render);
    assert(_output);
    if (output != _output)
    {
        // FIXME : if this situation happen
        weston_log("may be something need to careful happen.\n");
        assert(false);
    }
    _render->Repaint(damage);
}

void CustomBackend::WriteImage()
{
    _imageWriterDispatcher->Write(reinterpret_cast<uint8_t*>(_imageBuf), _imageBufLen);
}

void CustomBackend::StartRepaintLoop()
{
    assert(_output);
    struct timespec ts;
    weston_compositor_read_presentation_clock(_compositor, &ts);
    weston_output_finish_frame(_output, &ts, WP_PRESENTATION_FEEDBACK_INVALID);
}

} // namespace weston