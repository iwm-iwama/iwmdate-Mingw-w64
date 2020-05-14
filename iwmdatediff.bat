:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: ファイル名はソースと同じ
	set fn=%~n0
	set exec=%fn%.exe
	set op_link=-Os -lgdi32 -luser32 -lshlwapi
	set src=%fn%.c
	set lib=lib_iwmutil.a

:: Make ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	echo --- Compile -S ------------------------------------
	for %%s in (%src%) do (
		gcc.exe %%s -S %op_link%
		echo %%~ns.s
	)
	echo.

	echo --- Make ------------------------------------------
	for %%s in (%src%) do (
		gcc.exe %%s -c -Wall %op_link%
	)
	gcc.exe *.o %lib% -o %exec% %op_link%
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

	set s1="1970/12/10"
	set s2=now
	echo %s1% - %s2%

	%exec%
	%exec% %s2% %s1%
	%exec% %s2% %s1% -format="%%g%%y年%%m月%%d日 %%D通算日 %%W週%%w日"
	%exec% %s1% %s2%
	%exec% %s1% %s2% -format="%%g%%y年%%m月%%d日 %%D通算日 %%W週%%w日"
	%exec% %s1% %s2% -f="%%M通算月 %%H通算時 %%N通算分 %%S通算秒"

:: Quit ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:end
	echo.
	pause
	exit
