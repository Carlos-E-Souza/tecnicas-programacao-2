# Monitoramento de Logs

Repositorio da disciplina Tecnicas de Programacao 2 (CIC/UNB). Este projeto compila, testa e analisa um monitor de logs, com suporte a lint, analise estatica, cobertura e verificacao de vazamento de memoria.

## Pre-requisitos

Os comandos abaixo usam o ambiente Linux (Ubuntu/Debian). Instale os pacotes para compilar e executar todas as verificacoes de qualidade.

Atualize a lista de pacotes:

```bash
sudo apt update
```

Instale compilador, Make e debugger:

```bash
sudo apt install build-essential gdb
```

Instale ferramentas de analise e verificacao:

```bash
sudo apt install cppcheck valgrind
```

Instale o CppLint via pipx:

```bash
sudo apt install python3-pip pipx
pipx install cpplint
```

## Como compilar e testar

Entre na pasta do projeto e use o Makefile. O fluxo recomendado e limpar antes de compilar:

```bash
make clean
make
```

O comando `make` (ou `make all`) compila e executa os testes automaticamente.

## Comandos do Makefile

Execute com `make <alvo>`:

- `make` ou `make all`: compila tudo e roda os testes (`./testa_monitora_logs`).
- `make compile`: compila os binarios sem executar os testes.
- `make test`: executa apenas o binario de testes.
- `make cpplint`: checa o estilo conforme o Google C++ Style Guide.
- `make cppcheck`: analise estatica local do codigo.
- `make valgrind`: roda os testes com verificacao de vazamento de memoria.
- `make gcov`: gera cobertura de testes (.gcov).
- `make debug`: compila com `-g` e executa via gdb.
- `make clean`: remove artefatos de compilacao e cobertura.

## Documentacao e testes

- Tabela de decisao e expressoes regulares: ver [tabela_decisao.md](tabela_decisao.md).