#!/bin/bash

#####################################
#dynamic xml creation shell script
#####################################

langcode_mapping_table(){
	case $filename in
		"ar") lang_string="ar-ae";;
		"az") lang_string="az-az";;
		"bg") lang_string="bg-bg";;
		"ca") lang_string="ca-es";;
		"cs") lang_string="cs-cz";;
		"da") lang_string="da-dk";;
		"de_DE") lang_string="de-de";;
		"el_GR") lang_string="el-gr";;
		"en") lang_string="en-gb";;
		"en_PH") lang_string="en-ph";;
		"en_US") lang_string="en-us";;
		"es_US") lang_string="en-us";;
		"es_ES") lang_string="es-es";;
		"es_MX") lang_string="es-mx";;
		"et") lang_string="et-ee";;
		"eu") lang_string="eu-es";;
		"fi") lang_string="fi-fi";;
		"fr_CA") lang_string="fr-ca";;
		"fr_FR") lang_string="fr-fr";;
		"ga") lang_string="ga-ie";;
		"gl") lang_string="gl-es";;
		"hi") lang_string="hi-in";;
		"hr") lang_string="hr-hr";;
		"hu") lang_string="hu-hu";;
		"hy") lang_string="hy-am";;
		"is") lang_string="is-is";;
		"it_IT") lang_string="it-it";;
		"ja_JP") lang_string="ja-jp";;
		"ka") lang_string="ka-ge";;
		"kk") lang_string="kk-kz";;
		"ko_KR") lang_string="ko-kr";;
		"lt") lang_string="lt-lt";;
		"lv") lang_string="lv-lv";;
		"mk") lang_string="mk-mk";;
		"nb") lang_string="nb-no";;
		"nl_NL") lang_string="nl-nl";;
		"pl") lang_string="pl-pl";;
		"pt_BR") lang_string="pt-br";;
		"pt_PT") lang_string="pt-pt";;
		"ro") lang_string="ro-ro";;
		"ru_RU") lang_string="ru-ru";;
		"sk") lang_string="sk-sk";;
		"sl") lang_string="sl-si";;
		"sr") lang_string="sr-rs";;
		"sv") lang_string="sv-se";;
		"tr_TR") lang_string="tr-tr";;
		"uk") lang_string="uk-ua";;
		"uz") lang_string="uz-uz";;
		"zh_CN") lang_string="zh-cn";;
		"zh_HK") lang_string="zh-hk";;
		"zh_SG") lang_string="zh-sg";;
		"zh_TW") lang_string="zh-tw";;
		*) lang_string="NOT_EXIST";;
	esac
}

designid_mapping_table(){
	case $appid in
		"org.tizen.setting") design_id="IDS_ST_OPT_SETTINGS";;
		"setting-network-efl") design_id="IDS_ST_BODY_NETWORK";;
		"setting-font-efl") design_id="IDS_ST_BODY_FONT_TYPE";;
		"setting-ledindicator-efl") design_id="IDS_ST_BODY_LED_INDICATOR";;
		"setting-accessory-efl") design_id="IDS_COM_BODY_ACCESSORIES";;
		"setting-storage-efl") design_id="IDS_ST_BODY_STORAGE";;
		"setting-about-efl") design_id="IDS_ST_BODY_ABOUT_DEVICE";;
		"setting-security-efl") design_id="IDS_ST_BODY_SECURITY";;
		"setting-time-efl") design_id="IDS_ST_BODY_DATE_AND_TIME";;
		"setting-profile-efl") design_id="IDS_ST_HEADER_SOUND";;
		"setting-datausage-efl") design_id="IDS_MOBILEAP_MBODY_DATA_USAGE";;
		"setting-accessibility-efl") design_id="IDS_ST_HEADER_ACCESSIBILITY";;
		"setting-privacy-efl") design_id="IDS_ST_HEADER_PRIVACY";;
		"setting-motion-efl") design_id="IDS_ST_BODY_MOTION";;
		"setting-powersaving-efl") design_id="IDS_ST_MBODY_POWER_SAVING_MODE";;
		"setting-blockingmode-efl") design_id="IDS_ST_BODY_BLOCKING_MODE";;
		"setting-display-efl") design_id="IDS_ST_BODY_DISPLAY";;
		"setting-drivingmode-efl") design_id="IDS_ST_BODY_DRIVING_MODE";;
		"setting-personalpage-efl") design_id="IDS_ST_HEADER_PERSONAL_MODE";;
		"setting-palmmotion-efl") design_id="IDS_MTTRL_MBODY_PALM_MOTION";;
		"setting-easymode-efl") design_id="IDS_ST_BODY_EASY_MODE";;
		"setting-netrestrictionmode-efl") design_id="IDS_ST_HEADER_NETWORK_RESTRICTION_MODE_ABB_JPN";;
		"org.tizen.setting.display") design_id="IDS_ST_BODY_DISPLAY";;
		"org.tizen.setting.motion") design_id="IDS_ST_BODY_MOTION";;
		"org.tizen.setting.smartscreen") design_id="IDS_ST_HEADER_SMART_SCREEN";;
		"org.tizen.setting.language") design_id="IDS_ST_BODY_LANGUAGE_AND_KEYBOARD";;
		"org.tizen.setting.manageapplications") design_id="IDS_ST_BODY_MANAGE_APPLICATIONS";;
		"org.tizen.setting.developeroptions") design_id="IDS_ST_BODY_DEVELOPER_OPTIONS";;
		*) design_id="NOT_EXIST";;
	esac
}
##########################
# $1 = pofile folder
# $2 = src xml file
# $3 = dest xml file
##########################

tab=0;
rm $3;
echo "xml creation start"

#traversal in org.tizen.setting.sample.xml file
while read line; do
	#get entity
	ENTITY=`echo $line | cut -d'<' -f 2`

	#intent get & insert
	ENDOFLINE=`echo $line | cut -d'<' -f 3`
	CUT=${ENTITY:0:1}

	a=`expr ${#line} - 3`
	b=`expr ${#line} - 1`
	el=${ENTITY:$a:$b}

	if [[ "$CUT" = "/" ]]; then
		for ((i=0; i<$tab-1; i++)); do
			printf "\t" >> $3;
		done
		tab=`expr $tab - 1`
	else
		for ((i=0; i<$tab; i++)); do
			printf "\t" >> $3;
		done
		tab=`expr $tab + 1`
	fi

	if test -n "$ENDOFLINE"; then
		tab=`expr $tab - 1`
	fi
	if [[ "$CUT" = "?" ]]; then
		tab=`expr $tab - 1`
	fi
	if [[ "$el" = "/>" ]]; then
		tab=`expr $tab - 1`
	fi

	#read one line from src xml and write to dest xml
	printf "$line\n" >> $3;

	#get entity's tag and attribute
	TAG_NAME=${ENTITY%% *}
	TEMP=${ENTITY#* }
	ATTRIBUTES=${TEMP%*>}

	#If tag name is ui-application, add the label nodes
	if [[ $TAG_NAME = "ui-application" ]]; then

		ATTRIBUTES=`echo $ATTRIBUTES | cut -d' ' -f 1`
		#get attribute values
		eval $ATTRIBUTES
		echo "$appid creating..."

		designid_mapping_table;
		if [[ $design_id = "NOT_EXIST" ]]; then
			echo "pass appid  $appid"
			continue
		fi

		#traversal in po folder
		for pofile in `ls $1/*.po`;
		do
			filename=`basename $pofile .po`
			langcode_mapping_table;
			if [[ $lang_string = "NOT_EXIST" ]]; then
				echo "pass po file $pofile"
				continue
			fi

			#find string in file
			postrings=`grep -n $design_id $pofile | cut -d'"' -f 2`
			polines=`grep -n $design_id $pofile | cut -d':' -f 1`
			stringlist=( $postrings )
			linelist=( $polines )

			index=0
			for i in "${stringlist[@]}";
			do
				if [[ $i = $design_id ]]; then
					poline="${linelist[$index]}"
				fi
				index=`expr $index + 1`
			done

			poline=`expr $poline + 1`

			DESIGNSTR=`sed -n "$poline"p $pofile | cut -d'"' -f 2`

			#do remove "&" and <b200> and etc
			DESIGNSTR=`echo $DESIGNSTR | sed -e "s/\&/\&amp;/g"`
			DESIGNSTR=`echo $DESIGNSTR | sed -e "s/\xe2\x80\x8b//g"`
			DESIGNSTR=`echo $DESIGNSTR | sed -e "s/\xe2\x80\x8d//g"`

			for ((i=0; i<$tab; i++)); do
				printf "\t" >> $3;
			done
			printf "<label xml:lang=\"$lang_string\">$DESIGNSTR</label>\n" >> $3;
		done
	fi
done < $2

echo "xml create end"
