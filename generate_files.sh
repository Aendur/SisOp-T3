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
# 	done
# done

# Criar no subdiretório SD1, mais 3 subdiretórios: SD1.0, SD1.1 e SD1.2
for i in 0 1 2
do
	mkdir -vp /mnt/g/SD1/SD1.${i}
done

# Copiar no diretório raiz o máximo de arquivos possíveis, cada um com aproximadamente 512 Bytes.
for i in {1..66000}
do
	fname=$(printf "ROOT-FILE%05d.dat" $i)
	./a.out $fname 500
	mv $fname /mnt/g/$fname 2>/dev/null
	echo $fname
done

# # Copiar no subdiretório de SD1.0 o dobro do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 4 KB.
# for i in {1..132000} do
# 	./a.out /mnt/g/SD1/SD1.0${i}.txt 4000
# 	echo SD1.0/$i
# done
# Copiar no subdiretório de SD1.1 o triplo do número de arquivos existentes no diretório raiz, cada arquivo com aproximadamente 8 KB.
# Copiar em SD1.2 arquivos de vídeo, de forma a encher o pen-drive.

