#ifndef __SETTING_HELPER_H__
#define __SETTING_HELPER_H__

#include <setting.h>
bool is_searchmode_app(Cfg_Item_Type type);
extern bool vcui_doc_rcs_get_install_status(void);
extern bool is_selected_menu(Eina_List *list, const char *keystr);
extern void setting_main_genlist_handler_set(void *data, int viewtype);
extern void setting_main_genlist_handler_reset(void *data);
extern Eina_Bool setting_main_freeze_event_timer_cb(void *cb);

void setting_main_click_list_item_reset_data(void *data, Evas_Object *obj);;
void setting_main_destroy_ug_cb(ui_gadget_h ug, void *priv);
void setting_main_result_ug_cb(ui_gadget_h ug, app_control_h service, void *priv);
void setting_main_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode, void *priv);

#endif
