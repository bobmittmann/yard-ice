#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

export JTAGTOOL_ADDR=192.168.10.50

make D=$LVL && make D=$LVL jtagload && make D=$LVL logview

