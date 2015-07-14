#include <stdio.h>
#include <dbus/dbus.h>
#include <Elementary.h>
#include <appcore-efl.h>
#include <app.h>
#define TRACE(fmt, arg...) \
	do {\
		printf("\n[SETTING]\033[0;32m" fmt "\033[0m\t%s:%d\n", \
				##arg, (char*)(strrchr(__FILE__, '/')+1), __LINE__);\
	}while(0);


#define MAX_LOCAL_BUFSIZE 128
#define DBUS_PATH "/setting/dbus_handler"
#define DBUS_SIGNAL_INTERFACE "org.tizen.setting.signal"
#define DBUS_SIGNAL "test"



#define PACKAGE "com.samsung.dbus_server"

struct appdata
{
	Evas* evas;
	Evas_Object *win_main;
	Evas_Object *bg;
	Evas_Object *layout_main;
	Evas_Object *navi;


	Evas_Coord root_w;
	Evas_Coord root_h;
};

static DBusConnection *bus;

static DBusHandlerResult __dubs_msg_cb(DBusConnection* conn, DBusMessage* message, void* user_data)
{
    int my_pid = getpid();
    int sender_pid = 0;
    char* key = NULL;
    char* value = NULL;


    DBusError error;
    dbus_error_init(&error);

    if (dbus_message_is_signal(message, DBUS_SIGNAL_INTERFACE, DBUS_SIGNAL))
    {
        if (dbus_message_get_args(message, &error,
                DBUS_TYPE_UINT32, &sender_pid,
                DBUS_TYPE_STRING, &key,
                DBUS_TYPE_STRING, &value,
                DBUS_TYPE_INVALID) == FALSE)
        {
            TRACE("Fail to get data : %s", error.message);
            dbus_error_free(&error);
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }
    }

    if (sender_pid != 0 && my_pid != sender_pid)
    {
        //TRACE("[SERVER] received key : %s , value : %s ", key, value);
	if (key && '\0' != key[0] && value && '\0' != value[0] )
	{
	    //parase "key"
	    char *p = NULL;
	    char *id = NULL;
	    char *title = NULL;
	    p = strchr(key, '|');
	    if (p && '\0' != p[0])
	    {
		    *p = '\0';
		    id = key;
		    p++;
		    title = p;
	    }
	    //TRACE("[SERVER] title[%s], id[%s]", title, id);

	    //parase "key"
	    p = strchr(value, '|');
	    if (p && '\0' != p[0])
	    {
		    *p = '\0';
		    char *type = value;
		    p++;
		    if (type && 0 == strcmp("INT", type))
		    {
			int value = atoi(p);
			TRACE("[SERVER] The value of \"%s\"(its JasonID is %s) had changed to be :%d",
				title, id, value);
		    }
		    else
		    {
			char *value = p;
			TRACE("[SERVER] The value of \"%s\"(its JasonID is %s) had changed to be :%s",
				title, id, value);
		    }
	    }
	}
    }


    return DBUS_HANDLER_RESULT_HANDLED;
}

int __dbus_msg_listen(void)
{
    DBusError error;
    char rule[MAX_LOCAL_BUFSIZE];

    dbus_error_init(&error);
    bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
    if (!bus)
    {
        TRACE("[SERVER] Fail to connect to the D-BUS daemon: %s ", error.message);
        dbus_error_free(&error);
        return -1;
    }

    dbus_connection_setup_with_g_main(bus, NULL);
    snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);

    dbus_bus_add_match(bus, rule, &error);
    if (dbus_error_is_set(&error))
    {
        TRACE("[SERVER] Fail to rule set; %s ", error.message);
        dbus_error_free(&error);
        return -1;
    }

    if (dbus_connection_add_filter(bus, __dubs_msg_cb, NULL, NULL) == FALSE)
    {
        return -1;
    }

    TRACE("[SERVER] app signal initialized ");

    return 0;
}


int __dbus_msg_unlisten(void)
{
    DBusError error;
    char rule[MAX_LOCAL_BUFSIZE];

    dbus_error_init(&error);
    dbus_connection_remove_filter(bus, __dubs_msg_cb, NULL);
    snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);
    dbus_bus_remove_match(bus, rule, &error);

    if (dbus_error_is_set(&error))
    {
        TRACE("Fail to rule unset: %s ", error.message);
        dbus_error_free(&error);
        return -1;
    }

    dbus_connection_close(bus);
    TRACE("[SERVER] app signal finalized");

    return 0;
}

static void _quit_cb(void *data, Evas_Object* obj, void* event_info)
{
    Evas_Object *win = (Evas_Object *) data;
    elm_win_lower(win);
}

static Eina_Bool _keydown_event(void *data, int type, void *event)
{
    Ecore_Event_Key *ev = event;

    if (!strcmp(ev->keyname, "XF86Stop")) {
        elm_exit();
    }

    return EINA_TRUE;
}

static Evas_Object* create_win(const char *name)
{
    Evas_Object *eo;
    int w, h;

    eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
    if (!eo) return NULL;

    elm_win_title_set(eo, name);
    elm_win_borderless_set(eo, EINA_TRUE);
    elm_win_autodel_set(eo, EINA_TRUE);
    evas_object_smart_callback_add(eo, "delete,request", _quit_cb, NULL);
    //ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
    //evas_object_resize(eo, w, h);

    return eo;
}

static Evas_Object* _create_bg(Evas_Object *parent)
{
    Evas_Object *bg;

    bg = elm_bg_add(parent);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //elm_win_resize_object_add(parent, bg);
    evas_object_show(bg);

    return bg;
}

static Evas_Object* _create_layout_main(Evas_Object* parent)
{
    Evas_Object *layout;

    layout = elm_layout_add(parent);

    elm_layout_theme_set(layout, "layout", "application", "default");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //elm_win_resize_object_add(parent, layout);

    evas_object_show(layout);

    return layout;
}

static Evas_Object* _create_naviframe_layout(Evas_Object* parent)
{
    Evas_Object *navi;

    navi = elm_naviframe_add(parent);
    elm_object_part_content_set(parent, "elm.swallow.content", navi);

    evas_object_show(navi);

    return navi;
}


void list_item_touch_handler_exit(void* data, Evas_Object* obj, void* event_info)
{
    elm_exit();
}

static Evas_Object* _create_list_winset(Evas_Object* parent, struct appdata* ad)
{
    Evas_Object *li;
    int idx = 0;
    //struct _menu_item *menu_its;

    li = elm_list_add(parent);
    elm_list_mode_set(li, ELM_LIST_COMPRESS);

	elm_list_item_append( li, "QUIT", NULL, NULL, list_item_touch_handler_exit, ad);

    elm_list_go(li);
    return li;
}

static void _create_view_layout(struct appdata *ad)
{
    Evas_Object *list;
    Evas_Object *btn;

    list = _create_list_winset(ad->win_main, ad);
    btn = elm_button_add(ad->navi);
    elm_object_style_set(btn, "naviframe/end_btn/default");
    evas_object_smart_callback_add(btn, "clicked", _quit_cb, ad->win_main);
    elm_naviframe_item_push(ad->navi, _("Tizen UI - Server"), btn, NULL, list, NULL);
}



static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    //struct appdata *ad = (struct appdata *)data;
    //Evas_Event_Mouse_Down *ev = event_info;

}

static void _mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    //struct appdata *ad = (struct appdata *)data;

}

static void _mouse_move_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    //struct appdata *ad = (struct appdata *)data;
}

static void _window_resize_cb(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
    struct appdata *ad = (struct appdata *)data;
    evas_object_geometry_get(ad->win_main, NULL, NULL, &ad->root_w, &ad->root_h);
}

#if 0
static int _rotation_cb(enum appcore_rm mode, void *data)
{
    struct appdata *ad = (struct appdata *)data;
    int angle;

    switch (mode) {
        case APPCORE_RM_LANDSCAPE_NORMAL:
            angle = -90;
            break;

        case APPCORE_RM_LANDSCAPE_REVERSE:
            angle = 90;
            break;

        case APPCORE_RM_PORTRAIT_REVERSE:
            angle = 180;
            break;

        case APPCORE_RM_UNKNOWN:
        case APPCORE_RM_PORTRAIT_NORMAL:
        default:
            angle = 0;
            break;
    }

    elm_win_rotation_with_resize_set(ad->win_main, angle);
    // set_rotation_degree(angle);

    // This is need for customized rotation process.
    // rotate_for_winset(mode);

    return 0;
}
#endif

int app_create(void *data)
{
    struct appdata *ad = (struct appdata *) data;

    // Set the engine to opengl_x11
    //elm_config_engine_set("opengl_x11");

    ad->win_main = create_win(PACKAGE);

    evas_object_event_callback_add(ad->win_main, EVAS_CALLBACK_RESIZE, _window_resize_cb, ad);
    evas_object_show(ad->win_main);
    ad->evas = evas_object_evas_get(ad->win_main);
//    appcore_set_rotation_cb(_rotation_cb, ad);

    // set_edj_name(NULL);

	ad->bg = _create_bg(ad->win_main);
	ad->layout_main = _create_layout_main(ad->win_main);
	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_SHOW);

	ad->navi = _create_naviframe_layout(ad->layout_main);

	_create_view_layout(ad);

    evas_object_event_callback_add(ad->navi, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, ad);
    evas_object_event_callback_add(ad->navi, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb,  ad);
    evas_object_event_callback_add(ad->navi, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, ad);

    ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _keydown_event, NULL);

	// init dbus handler
	__dbus_msg_listen();

	return 0;
}

int app_terminate(void *data)
{
    //struct appdata *ad = data;

	// terminate handler
	__dbus_msg_unlisten();
    return 0;
}

int app_pause(void *data)
{
    //struct appdata *ad = data;

    return 0;
}

int app_resume(void *data)
{
    //struct appdata *ad = data;

    return 0;
}

int app_reset(service_h service, void* data)
{
	struct appdata* ad = data;

	if (ad->win_main)
		elm_win_activate(ad->win_main);

	return 0;
}


int main(int argc, char* argv[])
{
	struct appdata ad;

	struct appcore_ops ops = {
		.create = app_create,
		.terminate = app_terminate,
		.pause = app_pause,
		.resume = app_resume,
		.reset = app_reset,
	};

	ops.data = &ad;

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
