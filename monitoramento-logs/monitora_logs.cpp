// Copyright 2026
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

#include <cassert>
#include <fstream>
#include <string>

bool processa_lista_logs(char const * caminho_lista) {
  assert(caminho_lista != nullptr);

  std::ifstream lista(caminho_lista);
  if (!lista.is_open()) {
    return false;
  }

  std::string caminho_log;
  while (std::getline(lista, caminho_log)) {
    if (caminho_log.empty()) {
      continue;
    }
    std::ifstream log(caminho_log.c_str());
    if (!log.is_open()) {
      continue;
    }
  }

  return true;
}
