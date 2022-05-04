@REM Copiar 10 arquivos textos distintos, cada um com aproximadamente 40 KB Apagar o 3º, o 5º e o 7º arquivos criados.
CD SD1\SD1.2
D:\Documents\GitHub\SisOp-T3\filegen.exe mf40_.txt 40000 10 seq
del mf40_3.txt
del mf40_5.txt
del mf40_7.txt
cd ..
cd ..
