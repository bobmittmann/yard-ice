@echo off

set JTAGTOOL_ADDR=192.168.10.50
set BUILD_NAME=tcp_test
set TOOLS_DIR=..\..\..\tools

%TOOLS_DIR%\tftp_load.py -q -i -e -r  -a 0x08000000 -h %JTAGTOOL_ADDR% debug\%BUILD_NAME%.bin

if %ERRORLEVEL% NEQ 0 goto :EOF

%TOOLS_DIR%\dcclog -h %JTAGTOOL_ADDR% debug\%BUILD_NAME%.elf

:EOF

