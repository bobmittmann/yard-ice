@echo off

..\..\tools\tftp_load.py -q -i -e -r  -a 0x08000000 -h 192.168.10.50 debug\u2s-485.bin

if %ERRORLEVEL% NEQ 0 goto :EOF

..\..\tools\dcclog -h 192.168.10.50 debug\u2s-485.elf

:EOF

