#!/bin/bash

po_base_file="./resource/po/en.po"
header_dir="./setting-common/include"
header_file="setting-common-po-string.h"

write_header() {

cat << EOF > ${header_dir}/${header_file}
/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
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

#ifndef __SETTING_COMMON_PO_STRING_H__
#define __SETTING_COMMON_PO_STRING_H__

EOF
}

write_footer() {

cat << EOF >> ${header_dir}/${header_file}

#endif /* __SETTING_COMMON_PO_STRING_H__ */
EOF
}

main() {

    if [ ! -f $po_base_file ];
    then
        echo "Create $header_file : Fail!"
        echo "[Warning] ${po_base_file} file is not exist!"
        exit -1;
    fi


    write_header
    cat $po_base_file | grep -e "^msgid" | awk -F"\"" '{printf "#define %s\t\t\t\"%s\"\n", $2, $2}' | sort >> ${header_dir}/${header_file}
    write_footer

    echo "Create $header_file : Successed!"
}

main $*
