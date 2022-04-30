SET /A "NF_512=1000" 
SET /A "NF_4K=NF_512*2"
SET /A "NF_8K=NF_512*3"

@REM Apagar aleatoriamente, os arquivos deletados não podem ter sido criados sequencialmente, 50 arquivos de aproximadamente 1, 4 e 8 KB, em diferentes diretórios.
SETLOCAL ENABLEDELAYEDEXPANSION
@ECHO OFF
FOR /L %%I IN (1,1,50) DO (
	CALL:RAND 1 %NF_4K%
	echo Remove file SD1\SD1.0\sf4_!RAND_VAL!.txt
	del SD1\SD1.0\sf4_!RAND_VAL!.txt
)
FOR /L %%I IN (1,1,50) DO (
	CALL:RAND 1 %NF_8K%
	echo Remove file SD1\SD1.1\sf8_!RAND_VAL!.txt
	del SD1\SD1.1\sf8_!RAND_VAL!.txt
)
FOR /L %%J IN (0 1 2 3 4) DO (
	FOR /L %%I IN (1,1,10) DO (
		CALL:RAND 1 371
		echo Remove file SD%%J\file_!RAND_VAL!.txt
		del SD%%J\file_!RAND_VAL!.txt
	)
)
@ECHO ON

@REM Copiar 10 arquivos textos distintos, cada um com aproximadamente 40 KB Apagar o 3º, o 5º e o 7º arquivos criados.
CD SD1\SD1.2
D:\Documents\GitHub\SisOp-T3\filegen.exe mf20_.txt 20000 10 seq
D:\Documents\GitHub\SisOp-T3\filegen.exe mf40_.txt 40000 10 seq
cd ..
cd ..


GOTO:EOF

:RAND
SET /A RAND_VAL=%random% * (%2 - %1 + 1) / 32768 + %1
GOTO:EOF

