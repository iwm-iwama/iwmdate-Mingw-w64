@echo off
cls

:: "ƒx[ƒXƒtƒ@ƒCƒ‹–¼" + "_make.bat"
set batch=%~nx0
set src=%batch:_make.bat=%
set fn=
for /f "delims=. tokens=1,2" %%i in ("%src%") do (
	set fn=%%i%
)
set fn_exe=%fn%.exe
set cc=gcc.exe -std=c2x
set lib=lib_iwmutil2.a
set op_link=-Os -Wall -Wextra -lgdi32 -luser32 -lshlwapi

:: Assembler
::	echo --- Compile -S ------------------------------------
::	cp -f %fn%.s %fn%.s.old
::	for %%s in (%src%) do (
::		%cc% %%s -S %op_link%
::		echo %%~ns.s
::	)
::	echo.

:: Make
	echo --- Make ------------------------------------------
	%cc% %src% %lib% %op_link% -o %fn_exe%
	strip %fn_exe%
	echo.

:: Dump
::	cp -f %fn%.objdump %fn%.objdump.old
::	objdump -d -s %fn_exe% > %fn%.objdump
::	echo.

:: Test
	pause
	chcp 65001
	cls

	%fn_exe%

	set s1="1970/12/10"
	set s2=now

	%fn_exe% %s2% %s1%
	%fn_exe% %s2% %s1% -format="%%g%%y”N%%mŒ%%d“ú %%D’ÊZ“ú %%WT%%w“ú"
	%fn_exe% %s1% %s2%
	%fn_exe% %s1% %s2% -format="%%g%%y”N%%mŒ%%d“ú %%D’ÊZ“ú %%WT%%w“ú"
	%fn_exe% %s1% %s2% -f="%%Y’ÊZ”N %%M’ÊZŒ %%H’ÊZ %%N’ÊZ•ª %%S’ÊZ•b"

	echo.
	echo cjd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	%fn_exe% cjd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	echo jd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	%fn_exe% jd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"

:: Quit
	echo.
	pause
	exit
