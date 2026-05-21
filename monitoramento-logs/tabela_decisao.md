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


## Expressoes regulares de cobertura (caixa-aberta)

As expressoes abaixo descrevem caminhos internos do codigo. Cada expressao vira
um teste. Elas se alinham com as colunas da tabela de decisao quando aplicavel.

Legenda
- E: entrada valida.
- !E: entrada invalida.
- L: lista aberta com sucesso.
- !L: lista nao existe.
- V: linha vazia na lista.
- F: log existe.
- !F: log ausente.
- T: total_ existe.
- !T: total_ ausente.
- ZL: log vazio.
- ZT: total_ vazio.
- OK: linha de log valida.
- BAD: linha de log invalida.

R1: !L -> A1
- Teste: lista inexistente retorna falso (D1).

R2: L -> V -> A2 -> continua
- Teste: linha vazia e ignorada (D2).

R3: L -> !V -> !F -> A3 -> continua
- Teste: log ausente e ignorado (D3).

R4: L -> !V -> F -> !T -> !ZL -> A4
- Teste: cria total_ e copia log (D4).

R5: L -> !V -> F -> T -> !ZL -> !ZT -> OK* -> A5
- Teste: merge ordenado com total_ existente (D5).

R6: L -> !V -> F -> T -> ZL -> A6
- Teste: log vazio mantem total_ (D6).

R7: L -> !V -> F -> T -> !ZL -> ZT -> A4
- Teste: total_ vazio copia log (D7).

R8: L -> !V -> F -> (OK* + BAD+) -> A7 + A5
- Teste: linhas invalidas sao ignoradas, validas entram no merge (D8).

R9: L -> !V -> F -> !T -> OK -> A4
- Teste: regex parse sucesso cria total_.

R10: L -> !V -> F -> !T -> BAD+ -> A4
- Teste: regex parse falha gera total_ vazio.

R11: L -> !V -> F -> T -> !ZL -> !ZT -> total antes do log
- Teste: merge ordenado onde total vence.

R12: L -> !V -> F -> T -> !ZL -> !ZT -> log antes do total
- Teste: merge ordenado onde log vence.

R13: L -> !V -> F -> T -> !ZL -> !ZT -> empate (total primeiro)
- Teste: merge ordenado com empate preserva total primeiro.

R14: L -> !V -> F(dir_a) -> total_basename -> F(dir_b) -> mesmo total_
- Teste: logs com mesmo basename agregam em um total_.

R15: L -> !V -> F(log_a) -> A4 -> F(log_b) -> A4
- Teste: lista com multiplos arquivos processa em ordem.

R16: L -> !V -> caminho com '\\' -> normaliza -> F -> A4
- Teste: caminho com barras invertidas e normalizado.

R17: L -> !V -> F(log_e2e_a) + F(log_e2e_b) -> A4
- Teste: fim a fim com multiplos logs ordenados.
