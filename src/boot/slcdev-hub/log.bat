@echo off

set JTAGTOOL_ADDR=192.168.10.50
set BUILD_NAME=thinkos
set TOOLS_DIR=..\..\..\tools

%TOOLS_DIR%\dcclog -h %JTAGTOOL_ADDR% debug\%BUILD_NAME%.elf

:EOF

