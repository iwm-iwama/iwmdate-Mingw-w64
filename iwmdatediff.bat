:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: ファイル名はソースと同じ
	set fn=%~n0
	set src=%fn%.c
	set fn_exe=%fn%.exe
	set cc=gcc.exe
	set op_link=-Os -Wall -lgdi32 -luser32 -lshlwapi
	set lib=lib_iwmutil2.a

:: Make ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

::	echo --- Compile -S ------------------------------------
::	for %%s in (%src%) do (
::		%cc% %%s -S %op_link%
::		echo %%~ns.s
::	)
::	echo.

	echo --- Make ------------------------------------------
	for %%s in (%src%) do (
		echo %%s
		%cc% %%s -c -Wall %op_link%
	)
	%cc% *.o %lib% -o %fn_exe% %op_link%
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
	chcp 65001
	cls

	set s1="1970/12/10"
	set s2=now

	%fn_exe%
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

:: Quit ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:end
	echo.
	pause
	exit
