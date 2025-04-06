@echo off
cls

:: "ベースファイル名" + "_make.bat"
set batch=%~nx0
set src=%batch:_make.bat=%
set fn=
for /f "delims=. tokens=1,2" %%i in ("%src%") do (
	set fn=%%i%
)
set fn_exe=%fn%.exe
set cc=gcc.exe -std=c2x
set lib=lib_iwmutil2.a
set op_link=-Os -Wall -Wextra -Wformat=2 -lgdi32 -luser32 -lshlwapi

:: Assembler
	if exist "%fn%_old.s" (
		rm "%fn%_old.s"
	)
	if exist "%fn%.s" (
		mv "%fn%.s" "%fn%_old.s"
	)
::	for %%s in (%src%) do (
::		%cc% %%s -S %op_link%
::	)
::	ls -la *.s
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

	:: 文字化けするので、
	::   chcp 932
	::   ファイルフォーマット Shift_JIS
	:: としている。
	:: ※直接実行のときは問題ない
	chcp 932
	cls

	%fn_exe%

	set ymd="2000/1/1 00:00:00"

	%fn_exe% %ymd%

echo.
	%fn_exe% %ymd% -d=-36467
	%fn_exe% %ymd% -d=-36466
	%fn_exe% %ymd% -d=-36465

echo.
	set ymd="1582/10/15 00:00:00"

	%fn_exe% %ymd% -d=0
	%fn_exe% %ymd% -d=-1
	%fn_exe% %ymd% -d=-2
	%fn_exe% %ymd% -d=-3
	%fn_exe% %ymd% -d=-4
	%fn_exe% %ymd% -d=-5
	%fn_exe% %ymd% -d=-6
	%fn_exe% %ymd% -d=-7
	%fn_exe% %ymd% -d=-8
	%fn_exe% %ymd% -d=-9
	%fn_exe% %ymd% -d=-10
	%fn_exe% %ymd% -d=-11
	%fn_exe% %ymd% -d=-12

echo.
	set ymd="1582/10/4 00:00:00"

	%fn_exe% %ymd% -d=0
	%fn_exe% %ymd% -d=1
	%fn_exe% %ymd% -d=2
	%fn_exe% %ymd% -d=3
	%fn_exe% %ymd% -d=4
	%fn_exe% %ymd% -d=5
	%fn_exe% %ymd% -d=6
	%fn_exe% %ymd% -d=7
	%fn_exe% %ymd% -d=8
	%fn_exe% %ymd% -d=9
	%fn_exe% %ymd% -d=10
	%fn_exe% %ymd% -d=11
	%fn_exe% %ymd% -d=12

echo.
	set ymd="1582/09/10"

	%fn_exe% %ymd% -m=1
	%fn_exe% %ymd% -m=1 -d=-1
	%fn_exe% %ymd% -m=1 -d=-2
	%fn_exe% %ymd% -m=1 -d=-3
	%fn_exe% %ymd% -m=1 -d=-4
	%fn_exe% %ymd% -m=1 -d=-5
	%fn_exe% %ymd% -m=1 -d=-6
	%fn_exe% %ymd% -m=1 -d=-7
	%fn_exe% %ymd% -m=1 -d=-8
	%fn_exe% %ymd% -m=1 -d=-9
	%fn_exe% %ymd% -m=1 -d=-10

echo.
	set ymd="2000/1/1"

	%fn_exe% %ymd%
	%fn_exe% %ymd% -h=-36                -f="%%y年%%m月%%d日 %%h時%%n分%%s秒"
	%fn_exe% %ymd% -h=-36 -n=-90         -f="%%y年%%m月%%d日 %%h時%%n分%%s秒"
	%fn_exe% %ymd% -h=-36 -n=-90 -s=-300 -f="%%y年%%m月%%d日 %%h時%%n分%%s秒"

echo.
	set ymd="1582/10/4"

	%fn_exe% %ymd% -d=0 -f="%%y年%%m月%%d日(%%a) CJD:%%C経過日 JD:%%J経過日"
	%fn_exe% %ymd% -d=1 -f="%%y年%%m月%%d日(%%a) CJD:%%C経過日 JD:%%J経過日"

echo.
	%fn_exe% . -f="CJD:%%C経過日 JD:%%J経過日"

:: Quit
	echo.
	pause
	exit
