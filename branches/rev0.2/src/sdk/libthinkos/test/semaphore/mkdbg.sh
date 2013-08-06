#!/bin/sh

if [ "$1" = "" ]; then
	LVL="5"
else
	LVL=$1
fi

make D=$LVL && make D=$LVL jtagload && make D=$LVL logview

