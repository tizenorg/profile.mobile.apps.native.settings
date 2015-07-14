#!/bin/bash

#-----------------------------------------------------------------------------------
#
# sort the contents by msgid for all .po files
#
#-----------------------------------------------------------------------------------

file_list=`find . -name "*.po"`

for file in $file_list
do
	sed -r ':r;/(^|\n)$/!{$!{N;br}};s/\n/\v/g' $file | sort | sed 's/\v/\n/g' > $file.result

	sed '/^$/d' $file.result > temp9876
	sed '/^msgid/{x;p;x;}' < temp9876 > $file.result
	mv $file.result $file

done
rm temp9876
