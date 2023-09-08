:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: ファイル名はソースと同じ
	set fn=%~n0
	set src=%fn%.c
	set fn_exe=%fn%.exe
	set cc=gcc.exe
	set cc_op=-Os -Wall -lgdi32 -luser32 -lshlwapi
	set lib=lib_iwmutil2.a

:: Make ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

::	echo --- Compile -S ------------------------------------
::	for %%s in (%src%) do (
::		%cc% %%s -S %cc_op%
::		echo %%~ns.s
::	)
::	echo.

	echo --- Make ------------------------------------------
	for %%s in (%src%) do (
		echo %%s
		%cc% %%s -c -Wall %cc_op%
	)
	%cc% *.o %lib% -o %fn_exe% %cc_op%
	echo.

	:: 後処理
	strip %fn_exe%
	rm *.o

	:: 失敗
	if not exist "%fn_exe%" goto end

	:: 成功
	echo.
	pause

:: Test ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
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
	%fn_exe% %ymd% -h=-36                -f="%%y-%%m-%%d %%h:%%n:%%s"
	%fn_exe% %ymd% -h=-36 -n=-90         -f="%%y-%%m-%%d %%h:%%n:%%s"
	%fn_exe% %ymd% -h=-36 -n=-90 -s=-300 -f="%%y-%%m-%%d %%h:%%n:%%s"
echo.
	set ymd="1582/10/4"
	%fn_exe% %ymd% -d=0 -f="%%y-%%m-%%d(%%a) %%C %%J"
	%fn_exe% %ymd% -d=1 -f="%%y-%%m-%%d(%%a) %%C %%J"
echo.
	%fn_exe% . -f="CJD:%%C JD:%%J"

:: Quit ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:end
	echo.
	pause
	exit
