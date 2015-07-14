#!/bin/bash

grep -R IDS_ --include=*.{h,c} ./| grep  "IDS_[a-zA-Z_0-9]*" -o | sort | uniq > temp5123

while read line
do
	name=$line

	#echo "$name"
	result=`grep -R $name --include=*.po ./| wc -l`
	#echo " $name : ($result) "
	if [ "$result" -ne "0" ];then
		echo "$name : ($result)"
	else
		echo "ZERO--> $name : ($result)"
	fi
done < temp5123

rm temp5123
