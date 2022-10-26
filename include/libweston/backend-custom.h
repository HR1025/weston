#ifndef WESTON_COMPOSITOR_CUSTOM_H
#define WESTON_COMPOSITOR_CUSTOM_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <libweston/libweston.h>

#define WESTON_CUSTOM_BACKEND_CONFIG_VERSION 1

/**
 * @brief 渲染模式
 */
enum weston_custom_render_mode
{
	CUSTOM_RENDER_MODE_CPU = 0,  // 使用 pixman
	CUSTOM_RENDER_MODE_GPU,      // 使用 OepnGl (not implement)
};

/**
 * @brief 输出配置
 */
struct weston_custom_output_config
{
	uint32_t  height;     // 高
	uint32_t  width;      // 宽
	uint32_t  refresh;    // 刷新率 (Hz)
	uint32_t  scale;      // 放缩比例 (not implement)
};

struct weston_custom_backend_config {
	struct weston_backend_config         base;
	enum   weston_custom_render_mode     render_mode;
	struct weston_custom_output_config   output_config;
};

#ifdef  __cplusplus
}
#endif

#endif /* WESTON_COMPOSITOR_CUSTOM_H */