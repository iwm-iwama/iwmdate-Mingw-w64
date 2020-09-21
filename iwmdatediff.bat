:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: ƒtƒ@ƒCƒ‹–¼‚Íƒ\[ƒX‚Æ“¯‚¶
	set fn=%~n0
	set src=%fn%.c
	set exec=%fn%.exe
	set cc=gcc.exe
	set lib=lib_iwmutil.a
	set option=-Os -lgdi32 -luser32 -lshlwapi

:: Make ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	echo --- Compile -S ------------------------------------
	for %%s in (%src%) do (
		%cc% %%s -S %option%
		echo %%~ns.s
	)
	echo.

	echo --- Make ------------------------------------------
	for %%s in (%src%) do (
		%cc% %%s -g -c -Wall %option%
		objdump -S -d %%~ns.o > %%~ns.objdump.txt
	)
	%cc% *.o %lib% -o %exec% %option%
	echo %exec%

	:: Œãˆ—
	strip -s %exec%
	rm *.o

	:: ¸”s
	if not exist "%exec%" goto end

	:: ¬Œ÷
	echo.
	pause

:: Test ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	cls

	set s1="1970/12/10"
	set s2=now
	echo %s1% - %s2%

	%exec%
	%exec% %s2% %s1%
	%exec% %s2% %s1% -format="%%g%%y”N%%mŒ%%d“ú %%D’ÊZ“ú %%WT%%w“ú"
	%exec% %s1% %s2%
	%exec% %s1% %s2% -format="%%g%%y”N%%mŒ%%d“ú %%D’ÊZ“ú %%WT%%w“ú"
	%exec% %s1% %s2% -f="%%Y’ÊZ”N %%M’ÊZŒ %%H’ÊZ %%N’ÊZ•ª %%S’ÊZ•b"

	echo.
	echo cjd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	%exec% cjd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	echo jd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"
	%exec% jd . -f="g:%%g M:%%M D:%%D H:%%H N:%%N S:%%S W:%%W"

:: Quit ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:end
	echo.
	pause
	exit
