// Copyright 2026

#define CATCH_CONFIG_MAIN
#include "./catch.hpp"  // NOLINT(build/include_subdir)
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

TEST_CASE("Lista inexistente retorna falso", "[monitora_logs]") {
  REQUIRE_FALSE(processa_lista_logs("./nao_existe.txt"));
}
