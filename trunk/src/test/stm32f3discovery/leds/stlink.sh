#!/bin/sh

if [[ "$OSTYPE" =~ ^cygwin ]]; then
	WINPROG_PREFIX=/cygdrive/c/Program\ Files
	CYGWIN=yes
elif [[ "$OSTYPE" =~ ^msys ]]; then
	WINPROG_PREFIX=/d/Program\ Files
else
	echo "#Error: unsuportted OS: ${OSTYPE}"
	exit 1
fi

WINPROG_X86=${WINPROG_PREFIX}\ \(x86\)

if [[ -d "${WINPROG_X86}" ]]; then
	WINPROGDIR=${WINPROG_X86}
else
	WINPROGDIR=${WINPROG_PREFIX}
fi

STM32UTILDIR="${WINPROGDIR}"/STMicroelectronics/STM32\ ST-LINK\ Utility
STLINKDIR="${STM32UTILDIR}"/ST-LINK\ Utility
PATH=$PATH:"${STLINKDIR}"
STLINKCLI=ST-LINK_CLI.exe

#echo ${WINPROGDIR}
#echo ${STLINKDIR}

make -j 4 || exit 1;

UNIX_BINFILE=`make bin_path`

if [[ "$OSTYPE" =~ ^cygwin ]]; then
	BINFILE=`cygpath -w "${UNIX_BINFILE}"`
elif [[ "$OSTYPE" =~ ^msys ]]; then
	BINNAME=`basename ${UNIX_BINFILE}`
	BINDIR=`dirname ${UNIX_BINFILE}`
	BINFILE=`cd ${BINDIR}; pwd -W`"/${BINNAME}"
else
	BINFILE=${UNIX_BINFILE};
fi

#${STLINKCLI} -c SWD UR -P "${BINFILE}" 0x08000000 -Rst
${STLINKCLI} -P "${BINFILE}" 0x08000000 -Rst

