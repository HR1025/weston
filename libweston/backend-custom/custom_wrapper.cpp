/**
 * @file     custom_wrapper.cpp
 * @author   haorui
 * @brief    用于将 C++ 接口封装成 C 接口, 供函数指针绑定使用
 * @version  0.1
 * @date     2022-10-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "CustomBackend.h"

#include <assert.h>

#include <libweston/libweston.h>
#include <libweston/zalloc.h>

#ifdef  __cplusplus
extern "C" {
#endif
#include <libweston/backend.h>
#include <libweston/libweston-internal.h>
#ifdef  __cplusplus
}
#endif


/************************************************************* weston_windowed_output_api(BEGIN) ***************************************************/
// for more informationm , you can see libweston/windowed-output-api.h

static int custom_output_set_size(struct weston_output *base, int width, int height)
{
    weston_log("[custom_wrapper] set output size\n");
    weston_log("[custom_wrapper] height is: %d, width is: %d\n", height, width);
	weston::CustomBackend::Instance()->InitOuputConfig();
    return 0;
}

static int custom_head_create(struct weston_compositor *compositor, const char *name)
{
    assert(name);

    weston_log("[custom_wrapper] create custom weston head, name is: %s\n", name);

    struct weston_head* head;
	head = reinterpret_cast<struct weston_head *>(zalloc(sizeof(weston_head)));
	if (head == NULL)
		return -1;
    
	weston_head_init(head, name);
	weston_head_set_connection_status(head, true);

	/* Ideally all attributes of the head would be set here, so that the
	 * user has all the information when deciding to create outputs.
	 * We do not have those until set_size() time through.
	 */
	weston_compositor_add_head(weston::CustomBackend::Instance()->GetCompositor(), head);

	return 0;
}
/************************************************************* weston_windowed_output_api(END) ***************************************************/

/************************************************************* weston_backend(BEGIN) ***************************************************/
// for more informationm , you can see libweston/backend.h

static void custom_backend_destroy(struct weston_compositor* compositor)
{
	weston_log("[custom_wrapper] destory backend output\n");
	weston::CustomBackend::Instance()->UnitCompositor();
}

static void* custom_backend_repaint_begin(struct weston_compositor* compositor)
{
	// Hint : frequently print, used by debug
	// weston_log("[custom_wrapper] weston backend repaint begin.\n");
	return nullptr;
}

static void custom_backend_repaint_cancel(struct weston_compositor* compositor, void* repaint_data)
{
	weston_log("[custom_wrapper] weston backend repaint cancel, may be some errors happen...\n");
}

static int custom_backend_repaint_flush(struct weston_compositor* compositor, void* repaint_data)
{
	// Hint : frequently print, used by debug
	// weston_log("[custom_wrapper] weston backend repaint flush.\n");
	return 0;
}

static struct weston_output* custom_backend_create_output(struct weston_compositor *compositor, const char *name)
{
	weston_log("[custom_wrapper] create weston backend, name is: %s\n", name);
	weston::CustomBackend::Instance()->SetName(std::string(name));
	weston::CustomBackend::Instance()->InitOutput();
	return weston::CustomBackend::Instance()->GetOuput();
}
/************************************************************* weston_backend(END) ***************************************************/

/************************************************************* weston_output(BEGIN) ***************************************************/
// for more information , you can see libweston/libweston.h (struct weston_output)

static int custom_output_start_repaint_loop(struct weston_output *output)
{
	// Hint : frequently print, used by debug
	// weston_log("[custom_wrapper] weston output start repaint loop.\n");
	weston::CustomBackend::Instance()->StartRepaintLoop();
	return 0;
}

static int custom_output_repaint(struct weston_output *output, pixman_region32_t *damage, void *repaint_data)
{
	weston::CustomBackend::Instance()->Repaint(output, damage);
	weston::CustomBackend::Instance()->UpdateTimer();
	weston::CustomBackend::Instance()->WriteImage();
	return 0;
}

static void custom_output_destroy(struct weston_output *output)
{
	weston_log("[custom_wrapper] destroy weston output.\n");
	if (output->enabled)
	{
		weston::CustomBackend::Instance()->UninitOuput(true);
	}
}

static int custom_output_disable(struct weston_output *output)
{
	weston_log("[custom_wrapper] disable weston output.\n");
	if (output->enabled)
	{
		weston::CustomBackend::Instance()->UninitOuput();
	}
	return 0;
}

static int custom_output_enable(struct weston_output *output)
{
	weston_log("[custom_wrapper] enable weston output.\n");
	return weston::CustomBackend::Instance()->EnableOuput() == true ? 0 : -1;
}

/************************************************************* weston_output(END) ***************************************************/

/**
 * @note see libweston/backend-headless/headless.c finish_frame_handler for more information
 */
static int finish_frame_handler(void *data)
{
	weston_output* output = reinterpret_cast<weston_output*>(data);
	struct timespec ts;
	weston_compositor_read_presentation_clock(output->compositor, &ts);
	weston_output_finish_frame(output, &ts, 0);

	return 1;
}