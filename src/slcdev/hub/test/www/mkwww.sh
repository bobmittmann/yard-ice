#!/bin/sh

# 
# Build the html content converting .html files int .c files
#

TOOLS_DIR=../../../../tools


for IN in *.html ; do
	OUT=${IN}.c
	echo ${TOOLS_DIR}/bin2hex.py -z -H www.h -o ${OUT} ${IN}
	${TOOLS_DIR}/bin2hex.py -z -H www.h -o ${OUT} ${IN}
done

