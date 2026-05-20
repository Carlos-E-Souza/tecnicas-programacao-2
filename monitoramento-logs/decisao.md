# Tabela de decisao - Monitoramento de logs

Este documento descreve a tabela de decisao usada para derivar os testes de caixa-fechada.
Cada coluna vira um teste no arquivo de testes.

## Escopo da decisao (processar uma entrada da lista)

Regras (condicoes)
- C1: Lista de logs existe e pode ser lida.
- C2: Linha da lista esta vazia.
- C3: Arquivo de log da linha existe e pode ser lido.
- C4: Arquivo total_ correspondente existe.
- C5: Arquivo de log esta vazio.
- C6: Arquivo total_ esta vazio.
- C7: Existem linhas invalidas no arquivo de log.

Acoes
- A1: Retornar falso (erro fatal).
- A2: Ignorar linha vazia.
- A3: Ignorar log ausente e seguir.
- A4: Criar total_ e copiar registros do log.
- A5: Fazer merge ordenado log + total_.
- A6: Manter total_ como esta (nenhuma nova linha valida).
- A7: Ignorar linhas invalidas e processar as validas.

## Tabela de decisao (colunas -> testes)

| Coluna | C1 | C2 | C3 | C4 | C5 | C6 | C7 | A1 | A2 | A3 | A4 | A5 | A6 | A7 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| D1 Lista inexistente | N | - | - | - | - | - | - | X | - | - | - | - | - | - |
| D2 Linha vazia | S | S | - | - | - | - | - | - | X | - | - | - | - | - |
| D3 Log ausente | S | N | N | - | - | - | - | - | - | X | - | - | - | - |
| D4 Log existe, total ausente | S | N | S | N | N | - | N | - | - | - | X | - | - | - |
| D5 Log existe, total existe | S | N | S | S | N | N | N | - | - | - | - | X | - | - |
| D6 Log vazio, total existe | S | N | S | S | S | - | - | - | - | - | - | - | X | - |
| D7 Total vazio, log existe | S | N | S | S | N | S | N | - | - | - | X | - | - | - |
| D8 Linhas invalidas no log | S | N | S | - | N | - | S | - | - | - | - | X | - | X |

Observacoes
- O log e o total_ sao ordenados por data/hora, com merge estavel.
- Em empate de timestamp, a linha do total_ vem antes da linha do log novo.
- Duplicatas sao mantidas.
- Em D8, linhas invalidas sao ignoradas, mas linhas validas sao processadas normalmente.

## Mapeamento para testes

- D1: Lista inexistente retorna falso.
- D2: Linha vazia e ignorada.
- D3: Lista existe + log ausente retorna verdadeiro.
- D4: Lista existe + log existe + total ausente cria total_ com conteudo do log.
- D5: Lista existe + log existe + total existe faz merge ordenado.
- D6: Log vazio + total existe mantem total_.
- D7: Total vazio + log existe copia log para total_.
- D8: Linhas invalidas sao ignoradas e validas sao processadas.
