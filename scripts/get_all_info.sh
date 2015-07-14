#!/bin/bash

SDB=/usr/bin/sdb
HTML=report_information.html

function show_menu()
{
	echo "Get Information"
	echo "1. images"
	echo "2. sounds"
	read -p "Select options: " n

	case "$n" in
		1)
			MENU_NUM=1
			;;
		2)
			MENU_NUM=2
			;;
		*)
			;;
	esac
}

function print_menu()
{
	case "$MENU_NUM" in
		1)
			echo ""
			echo "*** Choose images format ***"
			echo "       1. png"
			echo "       2. jpeg"
			echo "       3. jpg"
			echo "       4. gif"
			echo "       5. all"
			read -p "Select options: " n

			    case "$n" in
	    		    1)
						IMG_FORMAT=1
						;;
					2)
						IMG_FORMAT=2
						;;
					3)
						IMG_FORMAT=3
						;;
					4)
						IMG_FORMAT=4
						;;
					5)
						IMG_FORMAT=5
						;;
					*)
						;;
				esac
			;;
		2)
			echo ""
			echo "*** Choose sounds format ***"
			echo "       1. ogg"
			echo "       2. mp3"
			echo "       3. all"
			read -p "Select options: " n

				case "$n" in
					1)
						SOUND_FORMAT=1
						;;
					2)
						SOUND_FORMAT=2
						;;
					3)
						SOUND_FORMAT=3
						;;
					*)
						;;
				esac
			;;
	esac
}

function find_resources
{
	sdb root on
	case "$MENU_NUM" in
		1)
			case "$IMG_FORMAT" in
				1)
					result1=`sdb shell "find / -name *.png > /opt/temp_info.txt"`
					;;
				2)
					result1=`sdb shell "find / -name *.jpeg > /opt/temp_info.txt"`
					;;
				3)
					result1=`sdb shell "find / -name *.jpg > /opt/temp_info.txt"`
					;;
				4)
					result1=`sdb shell "find / -name *.gif > /opt/temp_info.txt"`
					;;
				*)
					result1=`sdb shell "rm -rf /opt/temp_info.txt"`
					result1=`sdb shell "find / -name *.png >> /opt/temp_info.txt"`
					result1=`sdb shell "find / -name *.jpeg >> /opt/temp_info.txt"`
					result1=`sdb shell "find / -name *.jpg >> /opt/temp_info.txt"`
					result1=`sdb shell "find / -name *.gif >> /opt/temp_info.txt"`
					;;
			esac
			;;
		2)
			case "$SOUND_FORMAT" in
				1)
					result1=`sdb shell "find / -name *.ogg > /opt/temp_info.txt"`
					;;
				2)
					result1=`sdb shell "find / -name *.mp3 > /opt/temp_info.txt"`
					;;
				*)
					result1=`sdb shell "rm -rf /opt/temp_info.txt"`
					result1=`sdb shell "find / -name *.ogg > /opt/temp_info.txt"`
					result1=`sdb shell "find / -name *.mp3 >> /opt/temp_info.txt"`
					;;
			esac
			;;
		*)
			;;
	esac

	echo $result1

	result2=`sdb pull /opt/temp_info.txt`
	$result2

	if [ ! -d ./output ]
		then
		    mkdir -p ./output
	fi
}

function print_resources
{
	case "$MENU_NUM" in
		1)
			echo "<html><head><title> Get All Images from Target </title></head><body><table border=1>" >  $HTML

			while read line
			do
				name=$line
				result=`sdb pull $name output/$name`
				pkgname=`sdb shell "rpm -qf $name"`
				echo " $name : ($result) "
				echo "<tr><td>$name</td><td><img src=\"./output$name\"/></td><td>$pkgname</td></tr>" >> $HTML
			done < temp_info.txt

			echo "</table></body></html>" >> $HTML
			;;
		2)
			echo "<html><head><title> Get All Sounds from Target </title></head><body><table border=1>" >  $HTML
			echo "<tr>ogg files can not be opend in some browsers(IE)<br>MP3 files can not be opend in some browsers(firfox, opera)<p></tr>" >> $HTML

			while read line
				do
					name=$line
					result=`sdb pull $name output/$name`
					pkgname=`sdb shell "rpm -qf $name"`
					echo " $name : ($result) "
					echo "<tr><td>$name</td><td><audio controls=\"\"preload=\"\"><source src=\"./output$name\"/></audio></td><td>$pkgname</td></tr>" >> $HTML
				done < temp_info.txt

				echo "</table></body></html>" >> $HTML
			;;
	esac
}


#===================
	show_menu
	print_menu
	find_resources
	print_resources
	firefox $HTML
	rm temp_info.txt
