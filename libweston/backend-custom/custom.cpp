/**
 * @file     custom.cpp
 * @author   haorui
 * @brief    封装 SDK , 暴露 weston_backend_init 接口
 * @version  0.1
 * @date     2022-10-21
 * 
 * @note
 *           现有 weston 框架共有 drm-backend.so, headless-backend.so, wayland-backend.so 等多套不同的 backend 方案;
 *           在此基础上横向拓展出 custom-backend.so , 旨在方便用户能够根据自己的需求方便快速拓展 backend
 *           
 *           backend 是一个能够完整运行的后台终端,功能上相当于 x11; backend 的额外职责是负责将渲染数据输出到具体位置 (显示器,或者以流
 *           的方式进行远程输出), 并组合其他组件比如 input (输入事件), render (渲染器), 实现一个完整的解决方案; backend 以函数绑定的
 *           方式绑定到 compositer 上, compositor 作为一个状态机驱动 backend 的各种行为         
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef  __cplusplus
extern "C" {
#endif

#include <string.h>
#include <assert.h>

#ifdef  __cplusplus
}
#endif

#include <libweston/libweston.h>
#include <libweston/backend-custom.h>

#include <CustomBackend.h>

using namespace weston;

void custom_backend_create(struct weston_compositor* compositor, struct weston_custom_backend_config* config)
{
    auto setRenderType = [config]() -> void
    {
        if (config->render_mode == CUSTOM_RENDER_MODE_CPU)
        {
            CustomBackend::Instance()->SetRenderType(RenderResourceType::CPU);
        }
        else if (config->render_mode == CUSTOM_RENDER_MODE_GPU)
        {
            CustomBackend::Instance()->SetRenderType(RenderResourceType::GPU);
        }
        else
        {
            assert(false);
        }
    };
    CustomBackend::Instance()->SetCompositor(compositor);

    CustomBackend::Instance()->SetOutputConfig({config->output_config.height, config->output_config.width, config->output_config.refresh, PIXMAN_x8r8g8b8});

    setRenderType();
    if (!weston::CustomBackend::Instance()->Init())
    {
        weston_log("CustomBackend::Init fail, abort!!!\n");
        abort();
    }
}

WL_EXPORT int
weston_backend_init(struct weston_compositor* compositor,
                    struct weston_backend_config* config_base)
{
    /**
     * @brief 初始化配置
     */
    auto initConfig =
        [](struct weston_custom_backend_config & config, struct weston_backend_config * config_base)
    {
        if (sizeof(struct weston_custom_backend_config) != config_base->struct_size)
        {
            weston_log("weston_custom_backend_config is not the base class of weston_custom_backend_config!!!");
            abort();
        }
        memcpy(&config, config_base, config_base->struct_size);
        config.output_config.height   = config.output_config.height   == 0 ? 1080 : config.output_config.height;
        config.output_config.width    = config.output_config.width    == 0 ? 1920 : config.output_config.width;
        config.output_config.refresh  = config.output_config.refresh  == 0 ? 60   : config.output_config.refresh;
        config.output_config.scale    = config.output_config.scale    == 0 ? 1    : config.output_config.scale;
    };
    /**
     * @brief   打印配置 (for debug)
     */
    auto printConfig =
        [](struct weston_custom_backend_config & config)
    {
        weston_log("custom backend config:\n");
        weston_log("-- height is:%d\n", config.output_config.height);
        weston_log("-- width is:%d\n", config.output_config.width);
        weston_log("-- sclae is:%d\n", config.output_config.scale);
        weston_log("-- refresh is:%d\n", config.output_config.refresh);
        weston_log("-- render mode is:%s\n", config.render_mode == CUSTOM_RENDER_MODE_CPU ? "CPU" : "GPU");
    };
    struct weston_custom_backend_config config;
    initConfig(config, config_base);
    printConfig(config);
    custom_backend_create(compositor, &config);
    return 0;
}