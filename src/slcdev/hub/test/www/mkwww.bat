@echo off
rem 
rem Build the html content converting .html, .css and .js files int .c files
rem

set TOOLS_DIR=..\..\..\..\..\tools
set DIR=.

if "%~1"=="" goto :ALL

if exist %1\ goto :DIR

echo -- file %1
%TOOLS_DIR%\bin2hex.py -z -H www.h -o %1.c %1
goto :EOF

:DIR
echo -- dir: %1
cd %1

:ALL
for %%f in (*.html) do %TOOLS_DIR%\bin2hex.py -z -H www.h -o %%f.c %%f
for %%f in (*.css) do %TOOLS_DIR%\bin2hex.py -z -H www.h -o %%f.c %%f
for %%f in (*.js) do %TOOLS_DIR%\bin2hex.py -z -H www.h -o %%f.c %%f

:EOF
