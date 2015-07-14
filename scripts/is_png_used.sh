#!/bin/bash

png=report_png.txt

find . -name "*.png" -type f -printf "%f\n" > temp5321

while read line
do
name=$line
result=`grep -R $name --include=*.{h,c,edc,spec,xml,manifest} ./| wc -l`
#echo " $name : ($result) "
if [ "$result" -eq "0" ];then
echo "$name : ($result)"
fi
done < temp5321
rm temp5321

