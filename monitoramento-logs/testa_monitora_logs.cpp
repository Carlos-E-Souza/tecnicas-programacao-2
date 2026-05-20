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
