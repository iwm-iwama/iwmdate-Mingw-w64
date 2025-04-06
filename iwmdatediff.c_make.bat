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
::	echo --- Compile -S ------------------------------------
	if exist "%fn%_old.s" (
		rm "%fn%_old.s"
	)
	if exist "%fn%.s" (
		mv "%fn%.s" "%fn%_old.s"
	)

::	echo --- Compile gcc -S ------------------------------------------
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

	set s1="1970/12/10"
	set s2=now

	%fn_exe% %s2% %s1%
	%fn_exe% %s2% %s1% -format="%%g%%y年%%m月%%d日 %%D通算日 %%W週%%w日"
	%fn_exe% %s1% %s2%
	%fn_exe% %s1% %s2% -format="%%g%%y年%%m月%%d日 %%D通算日 %%W週%%w日"
	%fn_exe% %s1% %s2% -f="%%Y通算年 %%M通算月 %%H通算時 %%N通算分 %%S通算秒"

	echo.
	echo cjd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	%fn_exe% cjd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	echo jd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	%fn_exe% jd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"

:: Quit
	echo.
	pause
	exit
