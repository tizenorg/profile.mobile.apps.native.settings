#!/bin/bash

grep -R IDS_ --include=*.po ./| grep  "IDS_[a-zA-Z0-9_]*" -o | sort | uniq > temp5321

while read line
do
	name=$line
	result=`grep -R $name --include=*.{h,c} ./| wc -l`
	echo " $name : ($result) "
	#if [ "$result" -ne "0" ];then
	#	echo " $name : ($result) "
		#echo "$name"
	#fi
done < temp5321

