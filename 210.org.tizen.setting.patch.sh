#!/bin/sh
PATH=/bin:/usr/bin:/sbin:/usr/sbin

/usr/apps/org.tizen.setting/bin/setting_conf_util mig_cfg
chown app:app %{TZ_SYS_DATA}/setting/setting.cfg
