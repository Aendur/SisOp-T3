#!/bin/bash

# # Criar no diretório raiz 5 subdiretórios: SD0, SD1, SD2, SD3 e SD4
# for i in 0 1 2 3 4
# do
# 	mkdir -vp /mnt/g/SD${i}
# done

# Copiar em cada subdiretório 371 arquivos com aproximadamente 1 KB.
# for i in 0 1 2 3 4
# do
# 	for j in {1..371}
# 	do
# 		fname=$(printf "SD%d-FILE%03d_371.dat" $i $j)
# 		./a.out $fname 1000
# 		mv $fname /mnt/g/SD${i}/$fname 2>/dev/null
# 		echo $fname
#	 	sleep 0.05
# 	done
# done

# # Criar no subdiretório SD1, mais 3 subdiretórios: SD1.0, SD1.1 e SD1.2
# for i in 0 1 2
# do
# 	mkdir -vp /mnt/g/SD1/SD1.${i}
# done

# # Copiar no diretório raiz o máximo de arquivos possíveis, cada um com aproximadamente 512 Bytes.
# for i in {21840..21850}
# do
# 	fname=$(printf "ROOT-FILE%05d.dat" $i)
# 	./a.out $fname 500
# 	mv -v $fname /mnt/g/$fname 2>/dev/null
# 	#echo $fname
# 	sleep 0.05
# done

# Copiar no subdiretório de SD1.0 o dobro do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 4 KB.
# cd /mnt/g/SD1/SD1.0
# #for i in {1..43686}
# for i in {21801..22200}
# do
# 	fname=$(printf "SD1.0-FILE%05d.dat" $i)
# 	~/SisOp-T3/a.out $fname 4000
# done

# # Copiar no subdiretório de SD1.1 o triplo do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 8 KB.
#cd /mnt/g/SD1/SD1.1
#for i in {1..65529}
#do
#	fname=$(printf "SD1.1-FILE%05d.dat" $i)
#	~/SisOp-T3/a.out $fname 8000
#done


# Copiar em SD1.2 arquivos de vídeo, de forma a encher o pen-drive.
# cd /mnt/g/SD1/SD1.2
# for i in {1..6}
# do
# 	fname=$(printf "SD1.2-FILE%d_100MB.dat" $i)
# 	~/SisOp-T3/a.out $fname 100000000
# done

# cd /mnt/g/SD1/SD1.2
# fname=$(printf "SD1.2-FILE_30MB.dat" $i)
# ~/SisOp-T3/a.out $fname 30318000


## TOTAL FILES IN ROOT: 21843 + 5 DIRS
## TOTAL FILES IN SD1.0: 21844 (43686)
## TOTAL FILES IN SD1.1: 21844 (65529)
## 630 341 632 free bytes before SD1.2
## TOTAL FILES IN SD1.2: 7



