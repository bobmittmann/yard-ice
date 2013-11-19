#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

export JTAGTOOL_ADDR=192.168.0.50

make -j 6 D=$LVL && make D=$LVL jtagload && make D=$LVL logview

