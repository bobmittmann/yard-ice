#!/bin/bash

#
# Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
# 
# This file is part of the YARD-ICE.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/

#
#    File: tftp_load.sh
# Comment: JTAG tftp upload utility
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

PROG=`basename $0`
VER_MINOR=2
VER_MAJOR=0

TFTP=tftp
FILE=
DEFAULT_HOST="10.0.0.2"
ADDR="0x00100000"
RESET=0

show_usage() {
	echo "Usage: ${PROG} [OPTION] FILE"
	echo "Transfer FILE using the TFTP protocol"
	echo ""
    echo "  -a, --addr     upload address. Default to: ${ADDR}"
    echo "  -h, --host     remote host addr. Default to: ${DEFAULT_HOST}"
    echo "  -r, --reset    exec the reset script"
    echo "      --help     display this help and exit"
    echo "  -V, --version  output version information and exit"
	echo ""
}

show_version() {
	echo "${PROG} - version ${VER_MAJOR}.${VER_MINOR}"
	echo "Writen by Bob Mittmann - bob@boreste.com"
	echo "(C) Cpyright 2005-2010 - BORESTE (www.boreste.com)"
	echo
}

arg_missing(){
	echo "${PROG}: option '$1' requires an argument"
	echo "Try '${PROG} --help' for more information."
	exit 1
}

unknown_option(){
	echo "${PROG}: unrecognized option '$1'"
	echo "Try '${PROG} --help' for more information."
	exit 1
}

tftp_run_script() {
	HOST=$1
	SCRIPT=$2

#	echo "host: ${HOST}, script: ${SCRIPT}"

	RESULT=`${TFTP} ${HOST} << EOF
ascii
put ${SCRIPT}
quit
EOF`

	RESULT=`echo ${RESULT} | sed "s/tftp> *//g"`

#	echo "RESULT: '${RESULT}'"

	ERR=`echo ${RESULT} | grep "Error"`

	if [ ! "${ERR}" = "" ] ; then
		nl ${SCRIPT}
		echo ${RESULT}
		return 1
	fi

	ERR=`echo ${RESULT} | grep "timed out"`

	if [ ! "${ERR}" = "" ] ; then
		echo ${RESULT}
		return 1
	fi

	return 0
}

if [ $# = 0 ] ; then
	show_usage
	exit 1
fi

ARGS=
while [ "$1" ]; do
    case $1 in
	--version,-V)
		show_version
	    exit 0;;
	--help)
		show_usage
	    exit 0;;
	--reset|-r)
		RESET=1;;
	--host|-h)
		case $2 in -*) arg_missing $1; esac
		HOST=$2 
		shift;;
	--addr|-a)
		case $2 in -*) arg_missing $1; esac
		ADDR=$2
		shift;;
	-*)
		unknown_option $1
	    ;;
	*)
		[ "$ARGS" ] && ARGS="$ARGS $1" || ARGS=$1 
    esac
	shift
done

FILE=$ARGS

if [ "${FILE}" = "" ] ; then
	echo "${PROG}: you must specify a file to transfer"
	exit 1	
fi

if [ ! -f "${FILE}" ] ; then
	echo "${PROG}: ERROR: file not found: ${FILE}"
	exit 1
fi

if [ "${HOST}" = "" ] ; then
	if [ "${JTAGTOOL_ADDR}" = "" ] ; then
		HOST=${DEFAULT_HOST}
		echo default
	else
		HOST=${JTAGTOOL_ADDR}
		echo environment 
	fi
fi

BASENAME=`basename ${FILE}`
SIZE=`wc -c ${FILE} | cut -d ' ' -f 1`
#SCRIPT=`mktemp ${BASENAME}.tf.XXXXX`
INIT_SCRIPT=${PROG}-init.jtag
RST_SCRIPT=${PROG}-rst.jtag

echo " - File: '${BASENAME}'"
echo " - Size: ${SIZE} bytes"
echo " - Remote host: ${HOST}"
echo " - Upload address: ${ADDR}"
echo " - Initial script: ${INIT_SCRIPT} ..."

cat > ${INIT_SCRIPT} << EOF
connect
halt
init
erase ${ADDR} ${SIZE}
EOF

tftp_run_script ${HOST} ${INIT_SCRIPT} || exit 1

rm -f ${INIT_SCRIPT}

echo " - Loading binary file, wait..."

BEGIN_TM=$((`date +%s%N` / 1000000))

RESULT=`${TFTP} ${HOST} << EOF
bin
put ${FILE} ${ADDR} 
quit
EOF`
RESULT=`echo ${RESULT} | sed "s/tftp> *//g"`

ERR=`echo ${RESULT} | grep "Error"`
if [ ! "${ERR}" = "" ] ; then
	echo "ERROR: Unable to transfer data file."
	echo ${RESULT}
	exit 1
fi

END_TM=$((`date +%s%N` / 1000000))
TM=$(($END_TM - $BEGIN_TM))
SEC=$(($TM / 1000))
MS=$(($TM - $SEC * 1000))
SPEED=$((($SIZE * 1000) / $TM))
FTM=`printf "%d.%03d" $SEC $MS`

echo " - $SIZE bytes transferred in $FTM seconds ($SPEED bytes/sec)."

if [ ! "${RESET}" = "1" ] ; then
	exit 0
fi

echo " - Reset script..."

cat > ${RST_SCRIPT} << EOF
run
reset
con
EOF

tftp_run_script ${HOST} ${RST_SCRIPT} || exit 1

rm -f ${RST_SCRIPT}

echo

