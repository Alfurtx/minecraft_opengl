@echo off

setlocal

set CC=clang
set C_FLAGS=-std=c11 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
set C_FLAGS=%C_FLAGS% -Wno-unused-parameter -Wno-switch -Wno-unused-function
set C_LD_FLAGS=

endlocal
