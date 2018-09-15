#!/bin/sh

TOOLS_DIR=../../tools
PYTHON=python
if [ -z "$JTAGTOOL_ADDR" ]; then
	JTAGTOOL_ADDR=192.168.0.51
fi
# Collect ".bin" files in the positional parameters
set -- `ls debug/*.bin`
# Get the last one
for PROG_BIN; do true; done
# The corresponding .elf
PROG_ELF=${PROG_BIN%%.bin}.elf

# Collect ".bin" files in the positional parameters
set -- `ls ../boot/debug/*.bin`
# Get the last one
for THINKOS_BIN; do true; done
# The corresponding .elf
THINKOS_ELF=${THINKOS_BIN%%.bin}.elf

${PYTHON} ${TOOLS_DIR}/tftp_load.py -q -i -e -r  -a 0x08020000 \
	-h ${JTAGTOOL_ADDR} ${PROG_BIN} 

if [ $? = 0 ] ; then
	# Disable the halt debug mode by clearing C_DEBUGEN on DHCSR
	${PYTHON} ${TOOLS_DIR}/tftp_cmd.py -h ${JTAGTOOL_ADDR} 'disable debug'
	if [ $? = 0 ] ; then
		# Trace
		${TOOLS_DIR}/dcclog -h ${JTAGTOOL_ADDR} ${THINKOS_ELF} ${PROG_ELF}
	fi
fi

