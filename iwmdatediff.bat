:: Ini ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	@echo off
	cls

	:: �t�@�C�����̓\�[�X�Ɠ���
	set fn=%~n0
	set src=%fn%.c
	set exec=%fn%.exe
	set cc=gcc.exe
	set lib=lib_iwmutil.a
	set option=-Os -Wall -lgdi32 -luser32 -lshlwapi

:: Make ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	echo --- Compile -S ------------------------------------
	for %%s in (%src%) do (
		%cc% %%s -S %option%
		echo %%~ns.s
	)
	echo.

	echo --- Make ------------------------------------------
	for %%s in (%src%) do (
		%cc% %%s -g -c %option%
		objdump -S -d %%~ns.o > %%~ns.objdump.txt
	)
	%cc% *.o %lib% -o %exec% %option%
	echo %exec%
	echo.

	:: �㏈��
	strip %exec%
	rm *.o

	:: ���s
	if not exist "%exec%" goto end

	:: ����
	echo.
	pause

:: Test ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	cls

	set s1="1970/12/10"
	set s2=now

	%exec%
	%exec% %s2% %s1%
	%exec% %s2% %s1% -format="%%g%%y�N%%m��%%d�� %%D�ʎZ�� %%W�T%%w��"
	%exec% %s1% %s2%
	%exec% %s1% %s2% -format="%%g%%y�N%%m��%%d�� %%D�ʎZ�� %%W�T%%w��"
	%exec% %s1% %s2% -f="%%Y�ʎZ�N %%M�ʎZ�� %%H�ʎZ�� %%N�ʎZ�� %%S�ʎZ�b"

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
