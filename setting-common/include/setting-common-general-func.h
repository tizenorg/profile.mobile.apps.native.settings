/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef __SETTING_COMMON_GENERAL_FUNC_H__
#define __SETTING_COMMON_GENERAL_FUNC_H__

#ifdef _
#undef _
#undef N_
#undef S_
#endif

#include <appcore-efl.h>

#define EXPORT_PUBLIC __attribute__ ((visibility ("default")))

#ifdef _
#undef _
#undef N_
#undef S_
#endif

#include <setting-common-data-type.h>
#include <aul.h>
#include <ui-gadget-module.h>
#include <regex.h>

#ifndef _EDJ
#define _EDJ(o)				   elm_layout_edje_get(o)
#endif

#define SETTING_FILE_NAME_PATTERN	"[\\<>:;*\"|?/]"

#ifdef _
#undef _
#undef N_
#undef S_
#endif
#define _(s)			setting_gettext(s)
#define __(s, domainname)			setting_gettext2(s, domainname)
#define S_(s)		   setting_gettext(s)
#define U_(s)		   multilanguage_text_upper(s)

#define btn_type(obj) get_popup_btn_response_type(obj)

#define sncat(to, size, from) strncat(to, from, size-strlen(to)-1)

extern char *setting_file_basename(char *path);
extern int setting_file_attr_is_valid_name(const char *filename);

/*****/
extern char *get_pa_usb_connect_mode_str();
extern char *get_brightness_mode_str();
extern char *get_pa_screen_mode_str();
extern int adjust_backlight_time_value(int value);
extern char *get_pa_backlight_time_str();
extern char *get_pa_display_language_str();
extern char *get_pa_Wi_Fi_on_off_str();
extern char *get_Mobile_AP_on_off_str();
extern char *get_BT_on_off_str();
extern char *get_NFC_on_off_str();

extern char *get_pa_time_format_str();
extern char *get_pa_date_format_str();
extern char *get_pa_datetime_format_str();
extern char *get_pa_week_format_str();
extern char *get_pa_roaming_network_str();
extern char *get_pa_select_network();
extern char *get_pa_screen_lock_type_str();
extern char *get_pa_noti_readout_str();
extern double get_widgets_factor();
extern char *setting_gettext(const char *s);
extern char *modesyspopup_gettext(const char *s);
extern char *setting_gettext2(const char *s, const char *domainname);
extern bool is_digital_str(const char *cstr);
extern bool is_substr_ncase(const char *parentstr, const char *substr);

/* check if partern belongs to array..*/
extern bool is_string_belong_to_array(const char *partern, const char **array,
									  int array_num);
extern int safeStrCmp(const char *s1, const char *s2);
extern int safeStrNCmp(const char *s1, const char *s2, int len);
extern char *safeStrNCat(char *dst, const char *src, int maxlen);
extern char *safeCopyStr(char *dst, const char *src, int maxlen);
extern bool isEmptyStr(const char *str);
extern bool isSpaceStr(const char *str);
extern char *remove_first_substring(const char *parent, const char *pat);
extern int safeStrLen(const char *str);
extern char *safeStrStr(const char *str, const char *strSearch);
extern char *safeStrRStr(const char *str, const char *strSearch);
extern char *multilanguage_text_upper(const char *s);

extern bool get_substring_int(const char **ipStr, int *ipValue, char delim);
extern bool is_ip_string(const char *ipstr, char **output);
extern int setting_invoke_reset_function(char *ug_name, app_control_h service, void *ext);
extern int excuteCmd(char *exec_path, int option_num, ...);

extern char *get_icon_path(const char *package);

extern int is_slp_binary();
extern bool isEmulBin();
extern int get_popup_btn_response_type(Evas_Object *obj);
extern void clear_system_service_data();

extern char *cur_font_get();
extern void font_config_set(char *font_name);
extern void font_size_set();
extern char *_get_default_font();


extern int setting_set_i18n(char *pkgname, char *localedir);
extern int setting_set_i18n_force(char *pkgname, char *localedir);

extern int app_launcher(const char *pkg_name);
extern int app_group_launcher(const char *pkg_name);
extern char *substring(const char *str, size_t begin, size_t len);
extern bool check_blockingmode_sub_item();
extern int get_datausage_current_month_info(bool bisroaming, char *szUsed);
extern int get_current_datausage_this_month(double *total, double *szUsed);
extern bool get_tethering_status();
extern char *__get_font_size_str(int value);
extern char *format_backlight_time_str(char *backligt_time);
extern char *get_pa_backlight_time_value_str();

extern void setting_play_sound(const char *sound_file, int vol, void **mp_handle);





/* ui-gadget-1 */
extern int ug_create_cb(void (*create_cb)(char *, char *, char *, void *), void *user_data);
void setting_ug_create_cb(char *ug, char *mem, char *parent, void *user_data);
ui_gadget_h setting_ug_create(ui_gadget_h parent, const char *name, enum ug_mode mode, app_control_h service, struct ug_cbs *cbs);
void setting_ug_destroy(ui_gadget_h parent);
char *strcasestr(const char *haystack, const char *needle);
extern void setting_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
								 void *priv);


#endif				/* __SETTING_COMMON_GENERAL_FUNC_H__ */
