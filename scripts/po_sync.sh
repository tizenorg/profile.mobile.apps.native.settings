#!/bin/bash

STMS_gen_dir="/home/hj/share/tizen/po/Tizen_Platform_20141201_174449/settings-lite"

grep -R IDS_ --include=*.po ./ | grep  "IDS_[a-zA-Z0-9_]*" -o | sort | uniq > temp5321

output_dir="./po_list_dir"


if [ -d ${output_dir} ]
then
    rm ${output_dir} -rf
fi

if [ ! -d ${output_dir} ]
then
    mkdir -p ${output_dir}
fi

HTML=report_po_list.html

echo "<html><head><title> PO List </title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body><table border=1 style=\"border-collapse:collapse; border:1px gray solid;\">" > $HTML

echo "<tr align = \"center\"><td><b> NUM </td><td><b> REF_COUNT </td><td><b> SAME/DIFF </td><td><b> STRING NAME </td></tr>" >> $HTML

#| tr '\n' '<br>'
num1=0
while read line
do
	num2=0
	name=$line
	echo $name
	uuidgen_ret=`uuidgen`
	html_file_name=$output_dir/$uuidgen_ret.html
	txt_file_name=$output_dir/$uuidgen_ret.txt
	touch $txt_file_name
	awk -F\" '/"'${name}'"/{getline;print $2}' src/po/*.po > $txt_file_name
	ref_count=`grep -R ${name} --include=*.{h,c} ./| wc -l`


	# STMS strings
	stms_txt_file_name=$output_dir/$uuidgen_ret.stms
	stms_txt_file_name_diff_html=$output_dir/$uuidgen_ret.stms.diff.html
	awk -F\" '/"'${name}'"/{getline;print $2}' ${STMS_gen_dir}/*.po > $stms_txt_file_name

	diff -us $txt_file_name $stms_txt_file_name > temp_diff_result.diff
	result_code_diff=$?
	highlight --encoding=utf-8 temp_diff_result.diff -o $stms_txt_file_name_diff_html --syntax=html
	sleep 1
	echo "<html><head><title> PO List </title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body><table border=1 style=\"border-collapse:collapse; border:1px gray solid;\">" > $html_file_name

	while read line2
	do
		value=$line2
		#echo "------------------------------------------------"
		#echo "$value<br>"
		num2=$(($num2+1))
		echo "$num2. $value<br>" >> $html_file_name
	done < $txt_file_name

	echo "</body></html>" >> $html_file_name
	#rm $txt_file_name
	num1=$(($num1+1))



	if [[ ! $ref_count -eq 0 ]];
	then
		if [[ $result_code_diff -eq 0 ]];
		then
			echo "<tr><td align = \"center\">$num1</td><td align = \"center\">$ref_count</td><td bgcolor=\"green\" align = \"center\"><a href="$stms_txt_file_name_diff_html">SAME</a></td><td><a href="$html_file_name">$name</a></td></tr>" >> $HTML
		else

			if [ ! -s $stms_txt_file_name ] # is zero size ?
			then
				echo "<tr><td align = \"center\">$num1</td><td align = \"center\">$ref_count</td><td bgcolor=\"gray\" align = \"center\"><a href="$stms_txt_file_name_diff_html">SRC ONLY</a></td><td><a href="$html_file_name">$name</a></td></tr>" >> $HTML
			else
				echo "<tr><td align = \"center\">$num1</td><td align = \"center\">$ref_count</td><td bgcolor=\"red\" align = \"center\"><a href="$stms_txt_file_name_diff_html">DIFF</a></td><td><a href="$html_file_name">$name</a></td></tr>" >> $HTML
			fi
		fi
	fi
	done < temp5321

echo "</table></body></html>" >> $HTML

rm temp5321
rm temp_diff_result.diff

firefox $HTML
