@echo off

if not exist bin mkdir bin
set BUILD_FLAGS=/Zc:preprocessor /std:c11 /Fe:main.exe /Zi -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS /nologo -DDEBUG -fsanitize=address
set BUILD_FLAGS=%BUILD_FLAGS% /arch:AVX2 /O2

pushd bin & cl %BUILD_FLAGS% ..\src\main.c & popd
