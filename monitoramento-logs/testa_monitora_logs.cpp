// Copyright 2026

#define CATCH_CONFIG_MAIN
#include "./catch.hpp"  // NOLINT(build/include_subdir)
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

#include <cstdio>
#include <fstream>
#include <string>

TEST_CASE("Lista inexistente retorna falso", "[monitora_logs]") {
  // D1 / R1: !L -> A1
  // Lista nao existe; processa_lista_logs deve retornar falso.
  REQUIRE_FALSE(processa_lista_logs("./nao_existe.txt"));
}

TEST_CASE("Lista existe e log ausente retorna verdadeiro", "[monitora_logs]") {
  // D3 / R3: L -> !V -> !F -> A3 -> continua
  // Linha da lista aponta para log ausente; deve ignorar e retornar true.
  REQUIRE(processa_lista_logs("./listas/lista_com_log_ausente.txt"));
}

TEST_CASE("Lista existe e total ausente cria total_", "[monitora_logs]") {
  // D4 / R4: L -> !V -> F -> !T -> !ZL -> A4
  // Log existe e total_ nao; cria total_ com conteudo do log.
  std::remove("./total_log_existe.txt");

  REQUIRE(processa_lista_logs("./listas/lista_com_log_existe.txt"));

  std::ifstream total("./total_log_existe.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
}

TEST_CASE("Lista existe e total existe faz merge ordenado", "[monitora_logs]") {
  // D5 / R5: L -> !V -> F -> T -> !ZL -> !ZT -> OK* -> A5
  // Log e total_ existem; faz merge ordenado por data.
  std::remove("./total_log_existe.txt");
  std::ofstream total_base("./total_log_existe.txt");
  REQUIRE(total_base.is_open());
  total_base << "17/1/2026 14:17:46 Log do total" << "\n";
  total_base << "21/1/2026 18:55:38 Outro total";
  total_base.close();

  REQUIRE(processa_lista_logs("./listas/lista_com_log_existe.txt"));

  std::ifstream total("./total_log_existe.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "17/1/2026 14:17:46 Log do total");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "21/1/2026 18:55:38 Outro total");
}

TEST_CASE("Log vazio com total existente mantem total_", "[monitora_logs]") {
  // D6 / R6: L -> !V -> F -> T -> ZL -> A6
  // Log vazio; total_ existente permanece inalterado.
  std::remove("./total_log_vazio.txt");
  std::ofstream total_base("./total_log_vazio.txt");
  REQUIRE(total_base.is_open());
  total_base << "17/1/2026 14:17:46 Log do total" << "\n";
  total_base << "21/1/2026 18:55:38 Outro total";
  total_base.close();

  REQUIRE(processa_lista_logs("./listas/lista_com_log_vazio.txt"));

  std::ifstream total("./total_log_vazio.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "17/1/2026 14:17:46 Log do total");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "21/1/2026 18:55:38 Outro total");
}

TEST_CASE("Total vazio com log existente copia log", "[monitora_logs]") {
  // D7 / R7: L -> !V -> F -> T -> !ZL -> ZT -> A4
  // Total_ vazio; copia o conteudo do log.
  std::remove("./total_log_existe.txt");
  std::ofstream total_base("./total_log_existe.txt");
  REQUIRE(total_base.is_open());
  total_base.close();

  REQUIRE(processa_lista_logs("./listas/lista_com_log_existe.txt"));

  std::ifstream total("./total_log_existe.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
}

TEST_CASE("Linhas invalidas sao ignoradas e validas processadas", "[monitora_logs]") {
  // D8 / R8: L -> !V -> F -> (OK* + BAD+) -> A7 + A5
  // Linhas invalidas sao descartadas; validas entram no total_.
  std::remove("./total_log_misto.txt");

  REQUIRE(processa_lista_logs("./listas/lista_com_log_misto.txt"));

  std::ifstream total("./total_log_misto.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
}

TEST_CASE("Regex parse sucesso cria total_", "[monitora_logs]") {
  // R9: L -> !V -> F -> !T -> OK -> A4
  // Linha valida unica; total_ criado com a linha parseada.
  std::remove("./total_log_parse_ok.txt");

  REQUIRE(processa_lista_logs("./listas/lista_parse_ok.txt"));

  std::ifstream total("./total_log_parse_ok.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "1/2/2026 03:04:05 Log parse ok");
}

TEST_CASE("Regex parse falha gera total_ vazio", "[monitora_logs]") {
  // R10: L -> !V -> F -> !T -> BAD+ -> A4
  // Linha invalida unica; total_ criado sem conteudo.
  std::remove("./total_log_parse_fail.txt");

  REQUIRE(processa_lista_logs("./listas/lista_parse_fail.txt"));

  std::ifstream total("./total_log_parse_fail.txt");
  REQUIRE(total.is_open());
  REQUIRE(total.peek() == std::ifstream::traits_type::eof());
}

TEST_CASE("Merge ordenado onde total vence", "[monitora_logs]") {
  // R11: L -> !V -> F -> T -> !ZL -> !ZT -> total antes do log
  // Total_ tem timestamp menor; deve aparecer primeiro.
  std::remove("./total_log_total_wins.txt");
  std::ofstream total_base("./total_log_total_wins.txt");
  REQUIRE(total_base.is_open());
  total_base << "15/1/2026 12:00:00 Linha do total";
  total_base.close();

  REQUIRE(processa_lista_logs("./listas/lista_total_wins.txt"));

  std::ifstream total("./total_log_total_wins.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "15/1/2026 12:00:00 Linha do total");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Log do arquivo");
}

TEST_CASE("Merge ordenado onde log vence", "[monitora_logs]") {
  // R12: L -> !V -> F -> T -> !ZL -> !ZT -> log antes do total
  // Log tem timestamp menor; deve aparecer primeiro.
  std::remove("./total_log_log_wins.txt");
  std::ofstream total_base("./total_log_log_wins.txt");
  REQUIRE(total_base.is_open());
  total_base << "15/1/2026 12:00:00 Linha do total";
  total_base.close();

  REQUIRE(processa_lista_logs("./listas/lista_log_wins.txt"));

  std::ifstream total("./total_log_log_wins.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "14/1/2026 10:00:00 Linha do log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "15/1/2026 12:00:00 Linha do total");
}

TEST_CASE("Merge ordenado com empate preserva total primeiro", "[monitora_logs]") {
  // R13: L -> !V -> F -> T -> !ZL -> !ZT -> empate (total primeiro)
  // Empate de timestamp; total_ vem antes do log.
  std::remove("./total_log_empate.txt");
  std::ofstream total_base("./total_log_empate.txt");
  REQUIRE(total_base.is_open());
  total_base << "15/1/2026 12:00:00 Linha do total";
  total_base.close();

  REQUIRE(processa_lista_logs("./listas/lista_empate.txt"));

  std::ifstream total("./total_log_empate.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "15/1/2026 12:00:00 Linha do total");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "15/1/2026 12:00:00 Linha do log");
}

TEST_CASE("Logs com mesmo basename agregam em um total_", "[monitora_logs]") {
  // R14: L -> !V -> F(dir_a) -> total_basename -> F(dir_b) -> mesmo total_
  // Dois logs com mesmo basename; agregam em um unico total_.
  std::remove("./total_log1.txt");

  REQUIRE(processa_lista_logs("./listas/lista_multidir.txt"));

  std::ifstream total("./total_log1.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Log dir A");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "18/1/2026 11:34:21 Log dir B");
}

TEST_CASE("Lista com multiplos arquivos processa em ordem", "[monitora_logs]") {
  // R15: L -> !V -> F(log_a) -> A4 -> F(log_b) -> A4
  // Duas entradas na lista; ambos totais sao gerados.
  std::remove("./total_log_a.txt");
  std::remove("./total_log_b.txt");

  REQUIRE(processa_lista_logs("./listas/lista_duas_entradas.txt"));

  std::ifstream total_a("./total_log_a.txt");
  REQUIRE(total_a.is_open());
  std::string linha;
  REQUIRE(std::getline(total_a, linha));
  REQUIRE(linha == "10/1/2026 10:00:00 Log A");

  std::ifstream total_b("./total_log_b.txt");
  REQUIRE(total_b.is_open());
  REQUIRE(std::getline(total_b, linha));
  REQUIRE(linha == "11/1/2026 11:00:00 Log B");
}

TEST_CASE("Caminho com barras invertidas e normalizado", "[monitora_logs]") {
  // R16: L -> !V -> caminho com '\\' -> normaliza -> F -> A4
  // Caminho com barra invertida e normalizado antes de abrir.
  std::remove("./total_log_backslash.txt");

  REQUIRE(processa_lista_logs("./listas/lista_backslash.txt"));

  std::ifstream total("./total_log_backslash.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "12/1/2026 12:12:12 Log com barra invertida");
}

TEST_CASE("Fim a fim com multiplos logs ordenados", "[monitora_logs]") {
  // R17: L -> !V -> F(log_e2e_a) + F(log_e2e_b) -> A4
  // End-to-end com dois logs e normalizacao em um deles.
  std::remove("./total_log_e2e_a.txt");
  std::remove("./total_log_e2e_b.txt");

  REQUIRE(processa_lista_logs("./listas/lista_e2e.txt"));

  std::ifstream total_a("./total_log_e2e_a.txt");
  REQUIRE(total_a.is_open());
  std::string linha;
  REQUIRE(std::getline(total_a, linha));
  REQUIRE(linha == "05/1/2026 09:00:00 Log E2E A");
  REQUIRE(std::getline(total_a, linha));
  REQUIRE(linha == "15/1/2026 10:00:00 Log E2E A2");

  std::ifstream total_b("./total_log_e2e_b.txt");
  REQUIRE(total_b.is_open());
  REQUIRE(std::getline(total_b, linha));
  REQUIRE(linha == "10/1/2026 11:00:00 Log E2E B");
  REQUIRE(std::getline(total_b, linha));
  REQUIRE(linha == "20/1/2026 12:00:00 Log E2E B2");
}
