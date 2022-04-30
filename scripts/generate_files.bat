GOTO:LARGE

@REM Criar no diretório raiz 5 subdiretórios: SD0, SD1, SD2, SD3 e SD4
FOR %%I IN (0 1 2 3 4) DO (IF NOT EXIST SD%%I (MKDIR SD%%I))

@REM Criar em cada diretório 371 arquivos de aproximadamente 1 KB
FOR %%I IN (0 1 2 3 4) DO ((CD SD%%I) & (D:\Documents\GitHub\SisOp-T3\filegen.exe file_.txt 1000 371 rnd) & (CD ..))

@REM Criar no subdiretório SD1, mais 3 subdiretórios: SD1.0, SD1.1 e SD1.2
FOR %%I IN (0 1 2) DO (IF NOT EXIST SD1\SD1.%%I MKDIR SD1\SD1.%%I)

SET /A "NF_512=1000" 
SET /A "NF_4K=NF_512*2"
SET /A "NF_8K=NF_512*3"

@REM Copiar no diretório raiz o máximo de arquivos possíveis, cada um com aproximadamente 512 Bytes.
D:\Documents\GitHub\SisOp-T3\filegen.exe root.txt 500 %NF_512% rnd

@REM Copiar no subdiretório de SD1.0 o dobro do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 4 KB.
CD SD1\SD1.0
D:\Documents\GitHub\SisOp-T3\filegen.exe sf4_.txt 4000 %NF_4K% rnd
CD ..
CD ..

@REM Copiar no subdiretório de SD1.1 o triplo do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 8 KB.
CD SD1\SD1.1
D:\Documents\GitHub\SisOp-T3\filegen.exe sf8_.txt 8000 %NF_8K% rnd
CD ..
CD ..

:LARGE

@REM Copiar em SD1.2 arquivos de vídeo, de forma a encher o pen-drive.
@REM 225 368 064
@REM  25 364 480
CD SD1\SD1.2
D:\Documents\GitHub\SisOp-T3\filegen.exe lf_.txt  100000000 2 rnd
D:\Documents\GitHub\SisOp-T3\filegen.exe lf_3.txt  25364100 1 rnd
CD ..
CD ..
