# SisOp - Trabalho 3

Implementação do trabalho 3 para a disciplina de Sistemas Operacionais / UnB

Autor: Diogo César Ferreira

Link para o repositório: https://github.com/Aendur/SisOp-T3


## Compilação
Para compilar, executar o comando 
```
> nmake
```
ou
```
> nmake all
```
Serão criados os executáveis `diskexp.exe` e `filegen.exe` no diretório corrente.

## Diskexp
Explorador de sistema de arquivos FAT32. Para executar utilize o comando `diskexp X`, onde `X` é a letra referente ao *drive* que se deseja abrir.

**ATENÇÃO: ESTE PROGRAMA PODE DANIFICAR PERMANENTEMENTE OS DADOS NO DISCO**

## Filegen
Gerador de arquivos de tamanho arbitrário. Para executar utilize o comando `filegen [File name] [File size] [Num files] [rnd|seq]`.
Os argumentos do programa são:
1. (File name) O nome do arquivo a ser criado;
2. (File size) O tamanho do arquivo desejado, em bytes;
3. (Num files) A quatidade de arquivos a serem criados;
4. (rnd|seq) `rnd` para gerar arquivos com caracteres aleatórios ou `seq` para gerar arquivos com uma estrutura pré-definida.


## Pre-requisitos
Este programa deve ser executado no sistema operacional Windows, versão 10 ou superior.
Não há pre-requisitos adicionais de bibliotecas.
Para compliar é necessário executar o ambiente de desenvolvimento do MS Visual Studio (recomendado `MSVC++ 19.29 x64` (2019) ou mais recente).

