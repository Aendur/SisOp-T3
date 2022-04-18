# SisOp - Trabalho 2

Implementação do trabalho 2 para a disciplina de Sistemas Operacionais / UnB

Autor: Diogo César Ferreira

Link para o repositório: https://github.com/Aendur/SisOp-T2/tree/IPC

Este branch é referente à implementação usando **IPC**.
Para acesar a implementação com _threads_, acesse https://github.com/Aendur/SisOp-T2/tree/main

## Compilação
Para compilar, executar o comando 
```
$ make
```
ou
```
$ make all
```
Serão criados dois executáveis, nos caminhos `bin/server` e `bin/client`.

## Pre-requisitos
Este programa apresenta como pre-requisitos adicionais as seguintes bibliotecas:

Simple DirectMedia Layer
- `libsdl2-2.0-0`
- `libsdl2-dev`


## Servidor
É necessário executar inicialmente o programa servidor, que aguardará até que os processos clientes estejam prontos para se comunicar. O servidor aguardará até que N clientes se conectem,
onde N é o número definido no arquivo `settings_server.ini`, pelo parâmetro `NUM_PLAYERS`.

## Clientes
Os processos clientes irão contactar o processo servidor para obter os dados sobre a partida. Exatamente N processos clientes devem ser executados para que a partida se inicie,
onde N é o número definido no arquivo `settings_server.ini`, pelo parâmetro `NUM_PLAYERS`.

## Configuração da partida
Os processos do servidor e do cliente utilizam os parâmetros definidos respectivamente nos arquivos `settings_server.ini` e `settings_client.ini`. A especificação de como configurar esses
arquivos pode ser lida diretamente nos arquivos fornecidos. Para ler as configurações a partir de arquivos com outro nome, passar o nome do arquivo desejado como argumento da linha
de comando ao executar os programas:
```
$ server arquivo_de_config.ini
```
```
$ client arquivo_de_config.ini
```

## Dados adicionais
O programa foi testado utilizando-se o compilador/sistema operacional `g++ (Debian 11.2.0-16) 11.2.0` executando no WSL2.


