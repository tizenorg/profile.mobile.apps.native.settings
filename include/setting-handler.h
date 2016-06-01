#ifndef __SETTING_HANDLER_H__
#define __SETTING_HANDLER_H__

#include <setting.h>

mainlist_entry *settinig_drawer_hash_find(void *cb, char *search_str);
void settinig_drawer_hash_init(void *cb);
void setting_main_click_list_item_ug_cb(void *data, Evas_Object *obj,
		char *ug_to_load, app_control_h svc, struct ug_cbs *cbs);

#endif /* __SETTING_HANDLER_H__ */

