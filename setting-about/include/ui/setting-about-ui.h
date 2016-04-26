#ifndef __SETTING_ABOUT_UI_H__
#define __SETTING_ABOUT_UI_H__

#include <stdbool.h>
#include "ui/setting-about-ui-common.h"

extern bool setting_about_ui_init(void);
extern void setting_about_ui_deinit(void);

extern void setting_about_ui_display_attribute(ui_element_id uid, const char *str);

#endif /*__SETTING_ABOUT_UI_H__*/
