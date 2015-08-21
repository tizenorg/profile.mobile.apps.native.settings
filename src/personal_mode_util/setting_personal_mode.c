/*
 * setting_personal_mode
 *
 *
 *
 */
#include <setting-debug.h>
#include <setting-common-draw-widget.h>
#include <setting-common-general-func.h>
#include <stdio.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <vconf.h>
#include <app.h>
#include <ode.h>
#include <notification.h>
#include <ui-gadget.h>
#include <ui-gadget-module.h>

#define EXPORT_PUBLIC __attribute__((visibility("default")))

typedef struct _personal_mode_data {
	ui_gadget_h ug;
	Evas *evas;
	Evas_Object *win;
	Evas_Object *ly_main;

	Evas_Object *popup;
	Evas_Object *check;
} personal_mode_data;

static void personal_mode_terminate(void *data)
{
	SETTING_TRACE_BEGIN;

	ret_if(data == NULL);
	personal_mode_data *ad = (personal_mode_data *) data;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (ad->ly_main) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}

	if (ad->win) {
		evas_object_del(ad->win);
		ad->win = NULL;
	}

	elm_exit();
}

static void __del_cb(void *data, Evas_Object *obj, void *event)
{
	elm_exit();
}

void personal_mode_unmount(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	personal_mode_data *ad = (personal_mode_data *) data;

	/* unmount */
	if (ode_init() != 0) {
		SETTING_TRACE("change #1. init failed");
		setting_create_simple_popup(ad, ad->win, NULL, "Fail to change unlock method");
		return;
	}
	if (pse_unmount() != 0) {
		SETTING_TRACE("change #2. unmount failed");
		setting_create_simple_popup(ad, ad->win, NULL, "Fail to change unlock method");
		ode_deinit();
		return;
	}
	ode_deinit();

	vconf_set_bool(VCONFKEY_SETAPPL_PERSONAL_MODE_STATUS_BOOL, 0);
	notification_delete_all_by_type("org.tizen.setting.personal", NOTIFICATION_VARIABLE_TYPE_NONE);
	/* 1 line notification on the notification bar. */
	notification_status_message_post(_("IDS_ST_TPOP_PERSONAL_MODE_DISABLED_ABB"));
}

static Evas_Object *personal_mode_create_window(const char *name, bool transparent)
{
	SETTING_TRACE_BEGIN;

	Evas_Object *eo;
	int w, h;

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_conformant_set(eo, EINA_TRUE);

		if (transparent) {
			SETTING_TRACE_DEBUG("set transparent");

			elm_win_alpha_set(eo, EINA_TRUE);

			unsigned int opaqueVal = 1;
			Ecore_X_Atom opaqueAtom = ecore_x_atom_get("_E_ILLUME_WINDOW_REGION_OPAQUE");
			Ecore_X_Window xwin = elm_win_xwindow_get(eo);
			ecore_x_window_prop_card32_set(xwin, opaqueAtom, &opaqueVal, 1);
		}

		evas_object_smart_callback_add(eo, "delete,request", __del_cb, NULL);
		//ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
		//evas_object_resize(eo, w, h);

		elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
		elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_OPAQUE);

		if (elm_win_wm_rotation_supported_get(eo)) {
			const int rots[4] = {0, 90, 180, 270};
			elm_win_wm_rotation_available_rotations_set(eo, (const int *)&rots, 4);
		}
		SETTING_TRACE_DEBUG("win layer : %d", elm_win_layer_get(eo));
	}
	return eo;
}

static Evas_Object *personal_mode_create_layout(Evas_Object *win)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *eo = elm_layout_add(win);
	if (eo) {
		evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_layout_theme_set(eo, "layout", "application", "default");
		evas_object_show(eo);

		Evas_Object *conform = elm_conformant_add(win);
		evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(conform, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_win_resize_object_add(win, conform);
		elm_object_content_set(conform, eo);
		evas_object_show(conform);
	}
	return eo;
}

void personal_mode_destroy_ug_cb(ui_gadget_h ug, void *priv)
{
	personal_mode_data *ad = (personal_mode_data *) priv;
	if (!ad)
		return;		/*  do nothing if ad is NULL */

	/*elm_win_lower(ad->win); */

	if (ug) {
		setting_ug_destroy(ug);
		ad->ug = NULL;
	}
}

void personal_mode_end_ug_cb(ui_gadget_h ug, void *priv)
{
	if (!ug)
		return;
	personal_mode_data *ad = (personal_mode_data *) priv;
	if (!ad)
		return;		/*  do nothing if ad is NULL */
	personal_mode_terminate(ad);
}

void personal_mode_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
                                void *priv)
{
	Evas_Object *base;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
		case UG_MODE_FULLVIEW:
			evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
			                                 EVAS_HINT_EXPAND);
			ug_disable_effect(ug);
			evas_object_show(base);
			break;
		default:
			break;
	}
}

static void personal_mode_popup_cb(void *data, Evas_Object *obj, void *event)
{
	SETTING_TRACE_BEGIN;
	personal_mode_data *ad = data;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		if (ad->check) {
			int status = elm_check_state_get(ad->check);
			vconf_set_bool("db/setting/personal_off_popup_do_not_show_flag", status);
		}
		personal_mode_unmount(ad);
	}

	personal_mode_terminate(ad);
}

static bool personal_mode_create(void *data)
{
	SETTING_TRACE_BEGIN;

	return true;
}

static void personal_mode_service(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;

	personal_mode_data *ad = (personal_mode_data *)data;

	int 	curStatus = 0;
	int 	popupOption = 0;
	char 	*viewtype = NULL;

	/* get current personal mode */
	vconf_get_bool(VCONFKEY_SETAPPL_PERSONAL_MODE_STATUS_BOOL, &curStatus);

	/* get <do not show again> option */
	vconf_get_bool("db/setting/personal_off_popup_do_not_show_flag", &popupOption);

	/* check viewtype as "unlock_method" */
	app_control_get_extra_data(service, "viewtype", &viewtype);

	/* try to OFF */
	if (curStatus && !safeStrCmp(viewtype, "unlock_method")) {
		if (popupOption) {
			/* just unmount and exist */
			personal_mode_unmount(ad);
			personal_mode_terminate(ad);
		} else {
			/* create transparent window and popup */
			ad->win = personal_mode_create_window("personal_mode", 1);
			UG_INIT_EFL(ad->win, UG_OPT_INDICATOR_ENABLE);
			ad->evas = evas_object_evas_get(ad->win);

			ad->ly_main = personal_mode_create_layout(ad->win);
			evas_object_show(ad->win);
			elm_win_activate(ad->win);

			ad->popup = setting_create_popup_with_label_check(ad, ad->win, NULL,
			                                                  _("IDS_ST_POP_ANY_PERSONAL_CONTENT_WILL_NOT_BE_ACCESSIBLE_IN_NORMAL_MODE"),
			                                                  _("IDS_ST_BODY_DO_NOT_SHOW_AGAIN"), &(ad->check), personal_mode_popup_cb,
			                                                  0, 2, "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_OK");
			evas_object_show(ad->popup);
		}
	} else {
		/* create normal window and create ug with service */
		ad->win = personal_mode_create_window("personal_mode", 1);

		UG_INIT_EFL(ad->win, UG_OPT_INDICATOR_ENABLE);
		ad->evas = evas_object_evas_get(ad->win);

		ad->ly_main = personal_mode_create_layout(ad->win);
		evas_object_show(ad->win);
		elm_win_activate(ad->win);

		/* ug_create */
		struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
		if (!cbs) {
			personal_mode_terminate(ad);
			return;
		}
		cbs->layout_cb = personal_mode_layout_ug_cb;
		cbs->result_cb = NULL;
		cbs->destroy_cb = personal_mode_destroy_ug_cb;
		cbs->end_cb = personal_mode_end_ug_cb;
		cbs->priv = (void *)ad;

		ad->ug = setting_ug_create(NULL, "setting-personalpage-efl", UG_MODE_FULLVIEW, service, cbs);
	}
	FREE(viewtype);

	SETTING_TRACE_END;
}

static void personal_mode_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

/**
* The function is called when Setting begins run in forground from background
*/
static void personal_mode_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SETTING_TRACE_BEGIN;
	personal_mode_data ad;

	elm_init(argc, argv);

	ui_app_lifecycle_callback_s ops = {0,};
	ops.create = personal_mode_create;
	ops.terminate = personal_mode_terminate;
	ops.pause = personal_mode_pause;
	ops.resume = personal_mode_resume;
	ops.app_control = personal_mode_service;

	memset(&ad, 0x00, sizeof(personal_mode_data));

	int ret = ui_app_main(argc, argv, &ops, &ad);
	retvm_if(ret == -1, -1, "App returns -1");

	SETTING_TRACE_END;
	return 0;
}

