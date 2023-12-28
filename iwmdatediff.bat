:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: ƒtƒ@ƒCƒ‹–¼‚Íƒ\[ƒX‚Æ“¯‚¶
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

	:: Œãˆ—
	strip %fn_exe%
	rm *.o

	:: ¸”s
	if not exist "%fn_exe%" goto end

	:: ¬Œ÷
	echo.
	pause

:: Test ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	cls

	set s1="1970/12/10"
	set s2=now

	%fn_exe%
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

:: Quit ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:end
	echo.
	pause
	exit
