#!/bin/bash

PO=report_po.txt
temp=temp.txt

function show_menu()
{
	rm -rf $PO
	echo "=========== po check script ==========="
	echo " 1. is po string used?(po:O/src:X)"
	echo " 2. is po string available?(po:X/src:O)"
	echo "======================================="
	read -p "Select options: " n

    case "$n" in
        1)
			po_src_check
			;;
		2)
			src_po_check
			;;
		*)
			;;
	esac
}

function po_src_check
{
	grep -R IDS_ --include=*.po ./| grep  "IDS_[a-zA-Z0-9_]*" -o | sort | uniq > temp5321

	while read line
	do
		name=$line
		result=`grep -R $name --include=*.{h,c} ./| wc -l`
		echo " $name : ($result) " >> $temp
		echo " $name : ($result) "
	done < temp5321

	echo ""
	echo ""
	echo ""
	echo "=====result===========================" >> $PO

	sed -n '/(0)/p' $temp >> $PO

	rm temp5321
	rm $temp
	cat $PO
}

function src_po_check
{
	grep -R IDS_ --include=*.{h,c} ./| grep  "IDS_[a-zA-Z0-9_]*" -o | sort | uniq > temp5321

	while read line
	do
		name=$line
		result=`grep -R $name --include=*.po ./| wc -l`
		echo " $name : ($result) " >> $temp
		echo " $name : ($result) "
	done < temp5321

	echo ""
	echo ""
	echo ""
	echo "=====result===========================" >> $PO

	sed -n '/(0)/p' $temp >> $PO

	rm temp5321
	rm $temp
	cat $PO
}

show_menu
