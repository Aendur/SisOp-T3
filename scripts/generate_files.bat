@REM Criar no diretório raiz 5 subdiretórios: SD0, SD1, SD2, SD3 e SD4
@REM FOR %%I IN (0 1 2 3 4) DO (IF NOT EXIST SD%%I (MKDIR SD%%I))

@REM FOR %%I IN (0 1 2 3 4) DO (FOR /L %%J IN (1,1,371) DO (@echo %%I %%J))
@REM FOR %%I IN (0 1 2 3 4) DO ((CD SD%%I) & (D:\Documents\GitHub\SisOp-T3\filegen.exe FILE.TXT 1000 371) & (CD ..))

@REM Criar no subdiretório SD1, mais 3 subdiretórios: SD1.0, SD1.1 e SD1.2
@REM FOR %%I IN (0 1 2) DO (IF NOT EXIST SD1\SD1.%%I MKDIR SD1\SD1.%%I)

@REM Copiar no diretório raiz o máximo de arquivos possíveis, cada um com aproximadamente 512 Bytes.
@REM D:\Documents\GitHub\SisOp-T3\filegen.exe rootfile.txt 500 32765

@REM Copiar no subdiretório de SD1.0 o dobro do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 4 KB.
@REM CD SD1\SD1.0
@REM D:\Documents\GitHub\SisOp-T3\filegen.exe subfile4K_.txt 4000 65530 rem 21844
@REM CD ..
@REM CD ..

@REM Copiar no subdiretório de SD1.1 o triplo do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 8 KB.
@REM CD SD1\SD1.1
@REM D:\Documents\GitHub\SisOp-T3\filegen.exe subfile8K_.txt 8000 98295 rem 21844
@REM CD ..
@REM CD ..

@REM # Copiar em SD1.2 arquivos de vídeo, de forma a encher o pen-drive.
@REM CD SD1\SD1.2
@REM D:\Documents\GitHub\SisOp-T3\filegen.exe largefile0.txt 100000000 6
@REM D:\Documents\GitHub\SisOp-T3\filegen.exe largefile_fill.txt  86968000 1
@REM CD ..
@REM CD ..

@REM Apagar aleatoriamente, os arquivos deletados não podem ter sido criados sequencialmente, 50 arquivos de aproximadamente 1, 4 e 8 KB, em diferentes diretórios.
@REM SETLOCAL ENABLEDELAYEDEXPANSION
@REM FOR /L %%I IN (1,1,50) DO (
@REM 	CALL:RAND 10000 20000
@REM 	del SD1\SD1.0\subfile!RAND_VAL!.txt
@REM 	del SD1\SD1.1\subfile8K_!RAND_VAL!.txt
@REM )
@REM FOR /L %%I IN (1,1,10) DO (
@REM 	CALL:RAND 100 371
@REM 	del SD0\FILE!RAND_VAL!.TXT
@REM 	del SD1\FILE!RAND_VAL!.TXT
@REM 	del SD2\FILE!RAND_VAL!.TXT
@REM 	del SD3\FILE!RAND_VAL!.TXT
@REM 	del SD4\FILE!RAND_VAL!.TXT
@REM )
@REM GOTO:EOF
@REM :RAND
@REM SET /A RAND_VAL=%random% * (%2 - %1 + 1) / 32768 + %1
@REM GOTO:EOF

@REM Copiar 10 arquivos textos distintos, cada um com aproximadamente 40 KB Apagar o 3º, o 5º e o 7º arquivos criados.
@REM CD SD1\SD1.2
@REM D:\Documents\GitHub\SisOp-T3\filegen.exe mediumfile.txt 40000 10
@REM del mediumfile03.txt
@REM del mediumfile05.txt
@REM del mediumfile07.txt
@REM cd ..
@REM cd ..


@REM Fazer o undelete do 5º arquivo:
@REM a) de forma manual, utilizando algum visualizador de clusters ( mostrar como se faz ao professor em sala de aula; e
@REM b) de forma automática (o programa que você implementou na linguagem C/C++).
