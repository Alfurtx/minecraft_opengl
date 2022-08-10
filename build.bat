@echo off

setlocal

set CC=cl

set C_FLAGS=/W2 /Zi /nologo /DCGLM_ALL_UNALIGNED /MD
set C_FLAGS=%C_FLAGS% /wd5045 /wd5105 /wd4668 /wd4191 /wd4464 /wd4244 /wd4005
set C_LD_FLAGS=/Iw:\minecraft_opengl\libs\glad\include /Iw:\minecraft_opengl\libs\cglm\include /Iw:\minecraft_opengl\libs\glfw\include
set C_LINK_FLAGS=/link cglm.lib gdi32.lib user32.lib kernel32.lib shell32.lib glfw3.lib /LIBPATH:w:\minecraft_opengl\libs\cglm\win\x64\Release /LIBPATH:w:\minecraft_opengl\libs\glfw\lib-vc2022
set C_FILES=
set COUT_FLAGS=/Fe:minecraft_opengl.exe

set CLANG_FLAGS=-Wall -g -std=c11 -DCGLM_ALL_UNALIGNED -Wno-macro-redefined -Wno-deprecated-declarations -Wno-switch -Wno-unused-variable -Wno-unused-function
set CLANG_FLAGS=%CLANG_FLAGS% -Iw:\minecraft_opengl\libs\glad\include -Iw:\minecraft_opengl\libs\cglm\include -Iw:\minecraft_opengl\libs\glfw\include
set CLANG_FLAGS=%CLANG_FLAGS% -lgdi32 -luser32 -lkernel32 -lcglm -lglfw3 -lshell32
set CLANG_FLAGS=%CLANG_FLAGS% -Lw:\minecraft_opengl\libs\cglm\win\x64\Release -Lw:\minecraft_opengl\libs\glfw\lib-vc2022
set CLANG_FLAGS=%CLANG_FLAGS% -o minecraft_opengl.exe

rem get all source files in src/
pushd ".\src"
for /R %%c in (*.c) do call set C_FILES=%%c %%C_FILES%%
popd

rem get all source files in libs/glad
pushd ".\libs\glad"
for /R %%c in (*.c) do call set C_FILES=%%c %%C_FILES%%
popd

if not exist "bin" mkdir bin

pushd "bin"
%CC% %C_FLAGS% %C_LD_FLAGS% %C_FILES% %COUT_FLAGS% %C_LINK_FLAGS%
REM %CC% %CLANG_FLAGS% %C_FILES%
rem echo. %C_FILES%
popd

endlocal
