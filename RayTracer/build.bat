@echo off

if not exist build mkdir build

set COMPILE_FLAGS=/Zi /Od /nologo /I..\SDL2\include /Fe:main.exe /fsanitize=address
set LINK_FLAGS=..\SDL2\lib\SDl2main.lib ..\SDL2\lib\SDL2.lib Shell32.lib /link /SUBSYSTEM:CONSOLE

pushd build

cl %COMPILE_FLAGS% ..\src\main.c %LINK_FLAGS%

popd
