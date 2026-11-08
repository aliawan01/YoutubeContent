@echo off

if not exist bin mkdir bin
pushd bin & nvcc -g -O3 -o main.exe -I..\include -diag-suppress 550 ..\src\main.cu & popd
