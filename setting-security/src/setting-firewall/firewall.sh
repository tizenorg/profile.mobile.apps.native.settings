#!/bin/sh

DESC="Tizen firewall"
DAEMON=/usr/bin/$NAME
MJRUN=/usr/apps/org.tizen.setting/bin/firewall
FIREWALLSTART="30"
FIREWALLSTOP="31"

[ -x "$MJRUN" ] || exit 0

do_start() {
 	$MJRUN $FIREWALLSTART
		RETVAL=$?
		return "$RETVAL"

}

do_stop(){
		$MJRUN $FIREWALLSTOP
		RETVAL=$?
		return "$RETVAL"
}



prt_res() {
	RETVAL=$1

	case "$RETVAL" in
		77)
			echo "... done."
			RETVAL=0
			;;
		*)
			echo "... failed!"
			RETVAL=1
			;;
		esac
			return $RETVAL
}

case "$1" in
start)
echo "Starting $DESC"
do_start
prt_res $?

;;
stop)
echo "Stopping $DESC"
do_stop
prt_res $?
;;
*)
echo "Usage: firewall {start|stop}" >&2
exit 3
;;
esac
