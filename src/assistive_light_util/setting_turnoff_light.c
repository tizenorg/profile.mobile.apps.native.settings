/*
 * setting_turnoff_light
 *
 *
 *
 */
#include <setting-debug.h>
#include <stdio.h>
#include <Elementary.h>
#include <device.h>
#include <dd-led.h>
#include <feedback.h>
#include <vconf.h>
#include <app.h>
/*#include <livebox-service.h> */

#define EXPORT_PUBLIC __attribute__((visibility("default")))

static void turnofflight_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	elm_exit();
}

static bool turnofflight_create(void *data)
{
	SETTING_TRACE_BEGIN;
	return true;
}

static void turnofflight_service(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
	/* get current status of assistive light */
	int dev_value = 0;
	int ret;
	ret = device_flash_get_brightness(&dev_value);
	if (ret == DEVICE_ERROR_NOT_SUPPORTED) {
		SETTING_TRACE("assistive light is not supported by device");
		return;
	}
	/*turn off assistive light */
	if (dev_value) {
		ret = led_set_brightness_with_noti(0, 1);
		SETTING_TRACE("led_set_brightness_with_noti(0,1) returns %d", ret);
		vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, 0);

		/* haptic effect */
		if (feedback_initialize() == FEEDBACK_ERROR_NONE) {
			feedback_play_type(FEEDBACK_TYPE_VIBRATION, FEEDBACK_PATTERN_TAP);
			feedback_deinitialize();
		}
	}
	/* update accessibility dynamicbox */
	/*livebox_service_trigger_update("org.tizen.setting.accessibility.livebox", NULL, NULL, NULL, 1); */

	ui_app_exit();
	/*elm_exit(); */
	SETTING_TRACE_END;
}

static void turnofflight_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

/**
* The function is called when Setting begins run in forground from background
*/
static void turnofflight_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SETTING_TRACE_BEGIN;

	ui_app_lifecycle_callback_s ops = {0,};
	ops.create = turnofflight_create;
	ops.terminate = turnofflight_terminate;
	ops.pause = turnofflight_pause;
	ops.resume = turnofflight_resume;
	ops.app_control = turnofflight_service;

	int ret = ui_app_main(argc, argv, &ops, NULL);
	retvm_if(ret == -1, -1, "App returns -1");

	SETTING_TRACE_END;
	return 0;
}

