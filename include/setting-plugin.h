#ifndef _SETTING_PLUGIN_H_
#define _SETTING_PLUGIN_H_

#include <setting.h>
#include <Elementary.h>
#include <Eina.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifndef bool
#define bool int
#define TRUE 1
#define FALSE 0
#endif

#if 1
#define PLUGIN_INIT( context ) \
{\
	setting_drawer_list_init();\
	setting_dbus_handler_init( context );\
}
#define PLUGIN_FINI \
{\
	setting_dbus_handler_fini();\
	setting_drawer_list_fini();\
}
#else

#define PLUGIN_INIT
#define PLUGIN_FINI

#endif

extern int setting_dbus_handler_init(void* data);
extern int setting_dbus_handler_fini(void);
extern void setting_drawer_list_init();
extern void setting_drawer_list_fini();

typedef void* (*drawer_fp)(void *data, xmlNode *xmlObj);

// draw handler
typedef struct
{
	const char* type;			// ui type
	drawer_fp draw;
} Object_Drawer;

// draw handler
typedef struct
{
	xmlDocPtr doc;
	xmlNode *root;

	const char *cfg_file;
	//Evas_Object *win_get;
	Evas_Object *ly_main;
	Evas_Object *scroller;
	Evas_Object *navi_bar;
} Draw_Data;

typedef struct PluginNode PluginNode;


struct PluginNode
{
	char* plugin_path;
	Draw_Data *pd;
	Eina_Hash *ui_list;	// UI element to be udpated
};

extern PluginNode* setting_plugin_create();
extern bool setting_plugin_load(PluginNode* context, const char *cfg_file);
extern void setting_plugin_destroy(PluginNode* node);

#endif //_SETTING_PLUGIN_H_

