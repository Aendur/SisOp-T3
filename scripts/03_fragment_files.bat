SET /A "NF_512=1250" 
SET /A "NF_4K=NF_512*1"
SET /A "NF_8K=NF_512*1"

@REM Apagar aleatoriamente, os arquivos deletados não podem ter sido criados sequencialmente, 50 arquivos de aproximadamente 1, 4 e 8 KB, em diferentes diretórios.
SETLOCAL ENABLEDELAYEDEXPANSION
@ECHO OFF
FOR /L %%I IN (1,1,50) DO (
	CALL:RAND 1 %NF_4K%
	SET FILENAME=SD1\SD1.0\sf4k_!RAND_VAL!.txt
	echo Remove file !FILENAME!
	del !FILENAME!
)
FOR /L %%I IN (1,1,50) DO (
	CALL:RAND 1 %NF_8K%
	SET FILENAME=SD1\SD1.1\sf8k_!RAND_VAL!.txt
	echo Remove file !FILENAME!
	del !FILENAME!
)
FOR /L %%I IN (1,1,50) DO (
	CALL:RAND 0 4
	SET FOLDER=!RAND_VAL!
	CALL:RAND 1 371
	SET FILE=!RAND_VAL!
	SET FILENAME=SD!FOLDER!\file!FOLDER!_!FILE!.txt
	echo Remove file !FILENAME!
	del !FILENAME!
)
@ECHO ON

GOTO:EOF

:RAND
SET /A RAND_VAL=%random% * (%2 - %1 + 1) / 32768 + %1
GOTO:EOF

