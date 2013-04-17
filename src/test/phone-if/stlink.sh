#!/bin/sh

if [[ "$OSTYPE" =~ ^cygwin ]]; then
	WINPROG_PREFIX=/cygdrive/c/Program\ Files
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

make -j 4 || exit 1;

echo WINPROGDIR=${WINPROGDIR}
echo STM32UTILDIR=${STM32UTILDIR}
echo STLINKDIR=${STLINKDIR}

UNIX_BINFILE=`make bin_path`
BINFILE=`cygpath -w "${UNIX_BINFILE}"`

${STLINKCLI} -P "${BINFILE}" 0x08000000 -Rst

