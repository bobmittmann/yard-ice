@echo off

rem export JTAGTOOL_ADDR=192.168.10.50
rem make D=1 jtagload && 

..\..\tools\tftp_load.py -q -i -e -r  -a 0x08000000 -h 192.168.10.50 debug\thinkos.bin

if %ERRORLEVEL% NEQ 0 goto :EOF

..\..\tools\dcclog -h 192.168.10.50 debug\thinkos.elf

:EOF

