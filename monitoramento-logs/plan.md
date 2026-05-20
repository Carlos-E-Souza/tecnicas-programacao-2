Plan: Monitoramento de Logs com TDD
Vamos reutilizar a estrutura do projeto anterior para configurar Catch, criar o modulo de monitoramento (parse, merge, IO), atualizar o Makefile com cpplint/cppcheck/valgrind/gcov/gdb e documentar com Doxygen. A tabela de decisao e as expressoes regulares de cobertura guiam os testes; cada coluna/regex vira um teste em TDD com 30+ commits (teste antes do codigo).

Steps

Phase 0 - Baseline e requisitos: revisar o enunciado, definir a API minima, decidir o comportamento para arquivos ausentes, linhas invalidas, duplicatas e ordenacao; registrar isso na documentacao existente.
Phase 1 - Scaffolding: adaptar o Makefile existente, criar o novo arquivo de testes com Catch e pastas de fixtures; garantir alvos para teste, lint, cobertura, debug e analise estatica/dinamica.
Phase 2 - Tabela de decisao: montar tabela para o processamento por arquivo (lista existe? log existe? total existe? arquivos vazios? linhas invalidas?) e mapear cada coluna para um teste.
Phase 3 - Cobertura caixa-aberta: derivar regex de fluxo para funcoes chave (parse de linha, merge ordenado, processar um log, processar a lista) e mapear cada regex para testes; registrar a matriz de regex.
Phase 4 - TDD do parse/validacao: criar estrutura LogEntry, parser de data/hora/mensagem, comparador e regras de validacao; inserir assertivas de entrada/saida e comentarios Doxygen.
Phase 5 - TDD de merge e IO: ler logs e total, fazer merge estavel por timestamp, escrever total_; aplicar a tabela de decisao para arquivos ausentes e vazios.
Phase 6 - TDD de orquestracao: ler lista de logs e processar cada entrada; garantir prefixo total_ e agregacao por basename.
Phase 7 - Qualidade e docs: rodar cpplint/cppcheck/valgrind/gcov/gdb; corrigir alertas; gerar config Doxygen com exclusao de catch.hpp; atualizar doc com tabela de decisao, regex e checklist de revisao.
Phase 8 - Verificacao final: confirmar cobertura >=80% por modulo, rodar o Makefile completo e refatorar sem mudar comportamento.


Relevant files

Makefile — reutilizar alvos e flags para teste, lint, cobertura, debug e analises
catch.hpp — framework de testes
romanos.cpp — exemplo de helpers privados e validacao
romanos.hpp — exemplo de API publica simples
testa_romanos.cpp — exemplo de organizacao de testes
leia_me.txt — atualizar com doc do novo projeto e matrizes de teste


Verification

Compilar e rodar testes via Makefile (all, test).
Rodar cpplint e corrigir estilo Google C++.
Rodar cppcheck --enable=warning no codigo do projeto.
Rodar valgrind no executavel de testes.
Rodar gcov e confirmar >=80% por modulo.
Rodar gdb via alvo debug para validar fluxo.
Gerar Doxygen e confirmar exclusao de catch.hpp.


Decisions

Duplicatas sao mantidas; sem deduplicacao.
Linhas malformadas sao ignoradas com contagem; o processamento continua.
Normalizar barras invertidas para barras normais em Linux; o nome total_ usa o basename do caminho original.
Merge estavel por timestamp; em empate, preservar ordem (total antes do log novo).
Commit map (>=30, testes antes do codigo)

Commit 01 (test): adicionar arquivo de teste Catch com um teste falhando para lista inexistente.
Commit 02 (code): adicionar API minima que retorna erro para lista inexistente.
Commit 03 (test): teste da tabela de decisao para lista existe + log ausente.
Commit 04 (code): implementar leitura da lista e pulo de log ausente.
Commit 05 (test): teste da tabela de decisao para lista existe + log existe + total ausente.
Commit 06 (code): criar total_ com conteudo do log.
Commit 07 (test): teste da tabela de decisao para lista existe + log existe + total existe.
Commit 08 (code): implementar merge com total existente.
Commit 09 (test): teste da tabela de decisao para log vazio + total existe.
Commit 10 (code): no-op quando log vazio e total existe.
Commit 11 (test): teste da tabela de decisao para total vazio + log existe.
Commit 12 (code): copiar log direto quando total vazio.
Commit 13 (test): teste para linha invalida no log.
Commit 14 (code): parser rejeita/ignora linha invalida.
Commit 15 (test): teste de cobertura regex para parse sucesso.
Commit 16 (code): implementar parser e struct de timestamp.
Commit 17 (test): teste de cobertura regex para parse falha.
Commit 18 (code): implementar validacao e retorno de erro.
Commit 19 (test): teste de cobertura regex para merge onde total vence.
Commit 20 (code): implementar comparador e merge total-first.
Commit 21 (test): teste de cobertura regex para merge onde log novo vence.
Commit 22 (code): implementar merge log-first.
Commit 23 (test): teste de cobertura regex para empate no merge.
Commit 24 (code): implementar desempate estavel.
Commit 25 (test): teste para multi-diretorio com mesmo basename em um total_.
Commit 26 (code): agregar por basename para total_ unico.
Commit 27 (test): teste para multiplos arquivos na lista em ordem.
Commit 28 (code): implementar loop de orquestracao e codigos de retorno.
Commit 29 (test): teste para normalizacao de caminhos com barra invertida.
Commit 30 (code): implementar helper de normalizacao.
Commit 31 (test): teste end-to-end com entrada maior e ordenacao geral.
Commit 32 (code): refatorar, adicionar assertivas, comentarios Doxygen, atualizar Makefile.
Commit 33 (test): testes extras para fechar cobertura >=80%.
Commit 34 (code): refatoracao final sem mudar comportamento; atualizar documentacao.