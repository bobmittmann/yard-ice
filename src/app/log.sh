#!/bin/sh

TOOLS_DIR=../../tools
PYTHON=python
if [ -z "$JTAGTOOL_ADDR" ]; then
	JTAGTOOL_ADDR=192.168.10.51
fi
# Collect ".app" files in the positional parameters
set -- `ls debug/*.app`
# Get the last one
for PROG_APP; do true; done
# The corresponding .elf
PROG_ELF=${PROG_APP%%.app}.elf

# Collect ".bin" files in the positional parameters
set -- `ls ../boot/debug/*.bin`
# Get the last one
for THINKOS_BIN; do true; done
# The corresponding .elf
THINKOS_ELF=${THINKOS_BIN%%.bin}.elf

# Disable the halt debug mode by clearing C_DEBUGEN on DHCSR
${PYTHON} ${TOOLS_DIR}/tftp_cmd.py -h ${JTAGTOOL_ADDR} 'disable debug'
if [ $? = 0 ] ; then
	# Trace
	${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKOS_ELF} ${PROG_ELF}
fi

