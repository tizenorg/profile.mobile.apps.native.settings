#!/bin/sh
/usr/apps/org.tizen.setting/bin/setting_conf_util mig_cfg
/usr/bin/chown app:app %{TZ_SYS_DATA}/setting/setting.cfg
