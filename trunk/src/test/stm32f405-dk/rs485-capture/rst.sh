#!/bin/sh

if [[ "$OSTYPE" =~ ^cygwin ]]; then
	WINPROG_PREFIX=/cygdrive/c/Program\ Files
	CYGWIN=yes
elif [[ "$OSTYPE" =~ ^msys ]]; then
	WINPROG_PREFIX=/c/Program\ Files
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

${STLINKCLI} -Rst

