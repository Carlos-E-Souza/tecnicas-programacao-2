// Copyright 2026

#define CATCH_CONFIG_MAIN
#include "./catch.hpp"  // NOLINT(build/include_subdir)
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

#include <cstdio>
#include <fstream>
#include <string>

TEST_CASE("Lista inexistente retorna falso", "[monitora_logs]") {
  REQUIRE_FALSE(processa_lista_logs("./nao_existe.txt"));
}

TEST_CASE("Lista existe e log ausente retorna verdadeiro", "[monitora_logs]") {
  REQUIRE(processa_lista_logs("./fixtures/lista_com_log_ausente.txt"));
}

TEST_CASE("Lista existe e total ausente cria total_", "[monitora_logs]") {
  std::remove("./fixtures/total_log_existe.txt");

  REQUIRE(processa_lista_logs("./fixtures/lista_com_log_existe.txt"));

  std::ifstream total("./fixtures/total_log_existe.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
}

TEST_CASE("Lista existe e total existe faz merge ordenado", "[monitora_logs]") {
  std::remove("./fixtures/total_log_existe.txt");
  std::ofstream total_base("./fixtures/total_log_existe.txt");
  REQUIRE(total_base.is_open());
  total_base << "17/1/2026 14:17:46 Log do total" << "\n";
  total_base << "21/1/2026 18:55:38 Outro total";
  total_base.close();

  REQUIRE(processa_lista_logs("./fixtures/lista_com_log_existe.txt"));

  std::ifstream total("./fixtures/total_log_existe.txt");
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
  std::remove("./fixtures/total_log_vazio.txt");
  std::ofstream total_base("./fixtures/total_log_vazio.txt");
  REQUIRE(total_base.is_open());
  total_base << "17/1/2026 14:17:46 Log do total" << "\n";
  total_base << "21/1/2026 18:55:38 Outro total";
  total_base.close();

  REQUIRE(processa_lista_logs("./fixtures/lista_com_log_vazio.txt"));

  std::ifstream total("./fixtures/total_log_vazio.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "17/1/2026 14:17:46 Log do total");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "21/1/2026 18:55:38 Outro total");
}

TEST_CASE("Total vazio com log existente copia log", "[monitora_logs]") {
  std::remove("./fixtures/total_log_existe.txt");
  std::ofstream total_base("./fixtures/total_log_existe.txt");
  REQUIRE(total_base.is_open());
  total_base.close();

  REQUIRE(processa_lista_logs("./fixtures/lista_com_log_existe.txt"));

  std::ifstream total("./fixtures/total_log_existe.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
}

TEST_CASE("Linhas invalidas sao ignoradas e validas processadas", "[monitora_logs]") {
  std::remove("./fixtures/total_log_misto.txt");

  REQUIRE(processa_lista_logs("./fixtures/lista_com_log_misto.txt"));

  std::ifstream total("./fixtures/total_log_misto.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Este e um exemplo de log");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "20/1/2026 17:45:38 Outro exemplo de log");
}

TEST_CASE("Regex parse sucesso cria total_", "[monitora_logs]") {
  std::remove("./fixtures/total_log_parse_ok.txt");

  REQUIRE(processa_lista_logs("./fixtures/lista_parse_ok.txt"));

  std::ifstream total("./fixtures/total_log_parse_ok.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "1/2/2026 03:04:05 Log parse ok");
}

TEST_CASE("Regex parse falha gera total_ vazio", "[monitora_logs]") {
  std::remove("./fixtures/total_log_parse_fail.txt");

  REQUIRE(processa_lista_logs("./fixtures/lista_parse_fail.txt"));

  std::ifstream total("./fixtures/total_log_parse_fail.txt");
  REQUIRE(total.is_open());
  REQUIRE(total.peek() == std::ifstream::traits_type::eof());
}

TEST_CASE("Merge ordenado onde total vence", "[monitora_logs]") {
  std::remove("./fixtures/total_log_total_wins.txt");
  std::ofstream total_base("./fixtures/total_log_total_wins.txt");
  REQUIRE(total_base.is_open());
  total_base << "15/1/2026 12:00:00 Linha do total";
  total_base.close();

  REQUIRE(processa_lista_logs("./fixtures/lista_total_wins.txt"));

  std::ifstream total("./fixtures/total_log_total_wins.txt");
  REQUIRE(total.is_open());

  std::string linha;
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "15/1/2026 12:00:00 Linha do total");
  REQUIRE(std::getline(total, linha));
  REQUIRE(linha == "16/1/2026 13:27:46 Log do arquivo");
}
