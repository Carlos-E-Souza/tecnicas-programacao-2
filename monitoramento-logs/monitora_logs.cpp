// Copyright 2026
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

#include <cassert>
#include <fstream>

bool processa_lista_logs(char const * caminho_lista) {
  assert(caminho_lista != nullptr);

  std::ifstream lista(caminho_lista);
  if (!lista.is_open()) {
    return false;
  }

  return true;
}
