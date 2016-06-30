/*
 * setting_confutil
 *
 *
 *
	$ setting_confutil				// create a cfg file in default
	$ setting_confutil export		// export current status to a xml file
	$ setting_confutil timezone_init	// timezone init
 */
#include <stdio.h>

#include <setting-cfg.h>
#include <setting-debug.h>
#include <stdio.h>
#include <Elementary.h>
#include <setting-common-general-func.h>
#include <setting-common-data-slp-setting.h>
#include <unistd.h>
#include <system_settings.h>

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <setting-common-search.h>

static char *get_timezone();

static void get_gmt_offset(char *str_buf, int size)
{
	/* timezone string +/-<n> ex. +9, -1 */
	time_t t = time(0);		/* get unix time. sec. */
	struct tm *pdata;
	struct tm *data;

	pdata = localtime_r(&t, &data);		/* save time as structure. */
	setting_retm_if(!pdata, "data is NULL");
	pdata->tm_isdst = 0;			/* summer time, not applied. */
	time_t a = mktime(pdata);

	pdata = gmtime_r(&a, &data);
	setting_retm_if(!pdata, "data is NULL");
	pdata->tm_isdst = 0;			/* summer time, not applied. */
	time_t b = mktime(pdata);

	int gmtoffset_hour = (a - b) / 3600;	/* result : hour. */
	int gmtoffset_min = ((a - b) % 3600) / 60;	/* result : min. */
	if (gmtoffset_min != 0)
		gmtoffset_min = 30;

	snprintf(str_buf, size, "%+d:%02u", gmtoffset_hour, gmtoffset_min);
	SETTING_TRACE("szTimezone is of a valid format: GMT: %s", str_buf);
}

int generate_setting_cfg()
{
	if (0 == setting_cfg_create(false)) {
		SETTING_TRACE_ERROR("Error to create a new config file");
		return 0 ;
	}

	return 1;
}

int migrate_setting_cfg()
{
	SETTING_TRACE_BEGIN;
	if (0 == setting_cfg_migrate()) {
		SETTING_TRACE_ERROR("Fail to migrate config file");
		return 0 ;
	}
	SETTING_TRACE_END;
	return 1;
}

void timezone_init()
{
	char *tzpath = get_timezone();
	int ret = vconf_set_str(VCONFKEY_SETAPPL_TIMEZONE_ID, tzpath + 20);
	if (ret != 0)
		SETTING_TRACE("fail to set vconf");

	char str_buf[256] = {0, };
	get_gmt_offset(str_buf, 256);
	SETTING_TRACE(">>> time zone GMT string : %s", str_buf);
	g_free(tzpath);
}

void get_current_font()
{
	char *value = NULL;
	int retcode = system_settings_get_value_string(
			SYSTEM_SETTINGS_KEY_FONT_TYPE, &value);
	if (retcode != 0)
		SETTING_TRACE("fail to set SYSTEM_SETTINGS_KEY_FONT_TYPE");

	SETTING_TRACE(">>> get current font type : %s \n", value);
}

int status_fp(int total, int current, void *data)
{
	SETTING_TRACE(">> total : %d ---- current : %d ", total, current);
	return 0;
}


/**
sh-4.1# [_TZ_SYS_RW_APP]/org.tizen.setting/bin/setting_conf_util timezone_check
debug level init 1(1)
>>> time zone : /usr/share/zoneinfo/Asia/Seoul
*/
int main(int argc, char *argv[])
{
	elm_init(argc, argv);
	setting_set_i18n_force(SETTING_PACKAGE, SETTING_LOCALEDIR);

	if ((argc == 2) && (0 == strcmp(argv[1], "export_json")))
		setting_export_json(status_fp, NULL);
	else if ((argc == 2) && (0 == strcmp(argv[1], "import_json")))
		setting_import_json(status_fp, NULL);
	else if ((argc == 2) && (0 == strcmp(argv[1], "timezone_init")))
		timezone_init();
	else if ((argc == 2) && (0 == strcmp(argv[1], "get_current_font")))
		get_current_font();
	else if ((argc == 2) && (0 == strcmp(argv[1], "gen_cfg")))
		generate_setting_cfg();
	else if ((argc == 2) && (0 == strcmp(argv[1], "mig_cfg")))
		migrate_setting_cfg();

#if SETTING_SEARCH
	else if ((argc == 2) && (0 == strcmp(argv[1], "search_db_indexing"))) {
		/* app db search */
		__setting_init_search_index_app();
		__setting_init_search_index_module();
	}
#endif
	else {
		/* cfg create */
		/* TRUE or FALSE */
		setting_cfg_create(false);
	}
	return 0;
}

/* automatic */
static char *get_timezone()
{
	SETTING_TRACE_BEGIN;

	enum { BUFFERSIZE = 1024 };
	char buf[BUFFERSIZE] = {0, };
	ssize_t len = readlink(_TZ_SYS_ETC"/localtime", buf, sizeof(buf) - 1);

	if (len != -1) {
		buf[len] = '\0';
	} else {
		/* handle error condition */
		return NULL;
	}
	return g_strdup(buf);
}

