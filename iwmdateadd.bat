:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: ファイル名はソースと同じ
	set fn=%~n0
	set src=%fn%.c
	set exec=%fn%.exe
	set lib=lib_iwmutil.a
	set option=-Os -lgdi32 -luser32 -lshlwapi

:: Make ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	echo --- Compile -S ------------------------------------
	for %%s in (%src%) do (
		gcc.exe %%s -S %option%
		echo %%~ns.s
	)
	echo.

	echo --- Make ------------------------------------------
	for %%s in (%src%) do (
		gcc.exe %%s -g -c -Wall %option%
		objdump -S -d %%~ns.o > %%~ns.objdump.txt
	)
	gcc.exe *.o %lib% -o %exec% %option%
	echo %exec%

	:: 後処理
	strip -s %exec%
	rm *.o

	:: 失敗
	if not exist "%exec%" goto end

	:: 成功
	echo.
	pause

:: Test ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	cls

	%exec%

	set ymd="2000/1/1 00:00:00"

	%exec% %ymd%

echo.
	%exec% %ymd% -d=-36467
	%exec% %ymd% -d=-36466
	%exec% %ymd% -d=-36465
echo.
	set ymd="1582/10/15 00:00:00"

	%exec% %ymd% -d=0
	%exec% %ymd% -d=-1
	%exec% %ymd% -d=-2
	%exec% %ymd% -d=-3
	%exec% %ymd% -d=-4
	%exec% %ymd% -d=-5
	%exec% %ymd% -d=-6
	%exec% %ymd% -d=-7
	%exec% %ymd% -d=-8
	%exec% %ymd% -d=-9
	%exec% %ymd% -d=-10
	%exec% %ymd% -d=-11
	%exec% %ymd% -d=-12

echo.
	set ymd="1582/10/4 00:00:00"

	%exec% %ymd% -d=0
	%exec% %ymd% -d=1
	%exec% %ymd% -d=2
	%exec% %ymd% -d=3
	%exec% %ymd% -d=4
	%exec% %ymd% -d=5
	%exec% %ymd% -d=6
	%exec% %ymd% -d=7
	%exec% %ymd% -d=8
	%exec% %ymd% -d=9
	%exec% %ymd% -d=10
	%exec% %ymd% -d=11
	%exec% %ymd% -d=12
echo.
	set ymd="1582/09/10"
	%exec% %ymd% -m=1
	%exec% %ymd% -m=1 -d=-1
	%exec% %ymd% -m=1 -d=-2
	%exec% %ymd% -m=1 -d=-3
	%exec% %ymd% -m=1 -d=-4
	%exec% %ymd% -m=1 -d=-5
	%exec% %ymd% -m=1 -d=-6
	%exec% %ymd% -m=1 -d=-7
	%exec% %ymd% -m=1 -d=-8
	%exec% %ymd% -m=1 -d=-9
	%exec% %ymd% -m=1 -d=-10
echo.
	set ymd="2000/1/1"
	%exec% %ymd%
	%exec% %ymd% -h=-36                -f="%%y-%%m-%%d %%h:%%n:%%s"
	%exec% %ymd% -h=-36 -n=-90         -f="%%y-%%m-%%d %%h:%%n:%%s"
	%exec% %ymd% -h=-36 -n=-90 -s=-300 -f="%%y-%%m-%%d %%h:%%n:%%s"
echo.
	set ymd="1582/10/4"
	%exec% %ymd% -d=0 -f="%%y-%%m-%%d(%%a) %%C(%%E)"
	%exec% %ymd% -d=1 -f="%%y-%%m-%%d(%%a) %%C(%%E)"
echo.
	set ymd="1582/10/10"
	%exec% %ymd% -f="%%y-%%m-%%d(%%a) %%C(%%E)"

:: Quit ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:end
	echo.
	pause
	exit
