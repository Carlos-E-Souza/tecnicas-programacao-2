// Copyright 2026
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

namespace {

std::string monta_caminho_total(std::string const& caminho_log) {
  std::string::size_type pos = caminho_log.find_last_of("/\\");
  if (pos == std::string::npos) {
    return "total_" + caminho_log;
  }
  std::string dir = caminho_log.substr(0, pos + 1);
  std::string base = caminho_log.substr(pos + 1);
  return dir + "total_" + base;
}

bool copia_arquivo(std::ifstream* origem, std::ofstream* destino) {
  assert(origem != nullptr);
  assert(destino != nullptr);

  std::string linha;
  bool escreveu = false;
  while (std::getline(*origem, linha)) {
    *destino << linha;
    if (!origem->eof()) {
      *destino << "\n";
    }
    escreveu = true;
  }
  return escreveu;
}

}  // namespace

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

    std::string caminho_total = monta_caminho_total(caminho_log);
    std::ifstream total_leitura(caminho_total.c_str());
    if (!total_leitura.is_open()) {
      std::ofstream total_escrita(caminho_total.c_str());
      copia_arquivo(&log, &total_escrita);
      continue;
    }
  }

  return true;
}
