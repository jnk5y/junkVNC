@echo off
cls
ECHO ---------------------------------------------------------------------------
@rem Set the device you want to build for to 1
@rem Use Pixi to allow running on either device
set PRE=1
set PIXI=0
set DEBUG=0

@rem List your source files here
set SRC=../src/junkvnc_plugin.cpp ../src/sdlvnc/d3des.c ../src/sdlvnc/SDL_vnc.c ../src/SDL_rotozoom-1.6/SDL_rotozoom.c

@rem List the libraries needed
REM set LIBS=-lSDL -lGLESv2 -lpdl
REM set LIBS=-lSDL -lGLESv2 -lpdl -lSDL_image -lSDL_ttf
REM set LIBS=-lSDL -lGLESv2 -lpdl
set LIBS=-lSDL -lpdl

@rem Name your output executable
set OUTFILE=junkvnc_plugin

@rem Store project root
cd ..
set PROJECT=%CD%
cd windows

@REM clean
IF EXIST %OUTFILE% DEL %OUTFILE%

if %PRE% equ 0 if %PIXI% equ 0 goto :END

if %DEBUG% equ 1 (
   set DEVICEOPTS=-g
) else (
   set DEVICEOPTS=
)

if %PRE% equ 1 (
   set DEVICEOPTS=%DEVICEOPTS% -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
)

if %PIXI% equ 1 (
   set DEVICEOPTS=%DEVICEOPTS% -mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
)

ECHO PalmSDK = "%PALMPDK%"
ECHO DeviceOptions = "%DEVICEOPTS%"

REM ######### PRODUCTIN #########
REM * For production: Add "-s" to strip debug symbols. Or else it won't be accepted into the AppCatalog!
REM * Remember to undef DEBUG in SDL_vnc.c!
arm-none-linux-gnueabi-gcc %DEVICEOPTS% -s -o %OUTFILE% %SRC% "-I%PALMPDK%\include" "-I%PALMPDK%\include\SDL" "-L%PALMPDK%\device\lib" -Wl,--allow-shlib-undefined %LIBS%

ECHO ---------------------------------------------------------------------------
dir /n /c %OUTFILE%
REM pause

goto :EOF

:END
echo Please select the target device by editing the PRE/PIXI variable in this file.
exit /b 1

:END
ECHO ---------------------------------------------------------------------------
