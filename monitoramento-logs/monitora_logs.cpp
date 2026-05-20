// Copyright 2026
#include "./monitora_logs.hpp"  // NOLINT(build/include_subdir)

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct LogEntry {
  int ano;
  int mes;
  int dia;
  int hora;
  int minuto;
  int segundo;
  std::string linha;
};

std::string monta_caminho_total(std::string const& caminho_log) {
  std::string::size_type pos = caminho_log.find_last_of("/\\");
  if (pos == std::string::npos) {
    return "total_" + caminho_log;
  }
  std::string dir = caminho_log.substr(0, pos + 1);
  std::string base = caminho_log.substr(pos + 1);
  return dir + "total_" + base;
}

bool parse_linha_log(std::string const& linha, LogEntry* saida) {
  assert(saida != nullptr);

  int dia = 0;
  int mes = 0;
  int ano = 0;
  int hora = 0;
  int minuto = 0;
  int segundo = 0;
  if (std::sscanf(linha.c_str(), "%d/%d/%d %d:%d:%d", &dia, &mes, &ano,
                  &hora, &minuto, &segundo) != 6) {
    return false;
  }
  saida->dia = dia;
  saida->mes = mes;
  saida->ano = ano;
  saida->hora = hora;
  saida->minuto = minuto;
  saida->segundo = segundo;
  saida->linha = linha;
  return true;
}

std::vector<LogEntry> carrega_entradas(std::ifstream* arquivo) {
  assert(arquivo != nullptr);

  std::vector<LogEntry> entradas;
  std::string linha;
  while (std::getline(*arquivo, linha)) {
    LogEntry entrada;
    if (parse_linha_log(linha, &entrada)) {
      entradas.push_back(entrada);
    }
  }
  return entradas;
}

bool vem_antes(LogEntry const& esquerda, LogEntry const& direita) {
  if (esquerda.ano != direita.ano) return esquerda.ano < direita.ano;
  if (esquerda.mes != direita.mes) return esquerda.mes < direita.mes;
  if (esquerda.dia != direita.dia) return esquerda.dia < direita.dia;
  if (esquerda.hora != direita.hora) return esquerda.hora < direita.hora;
  if (esquerda.minuto != direita.minuto) return esquerda.minuto < direita.minuto;
  return esquerda.segundo < direita.segundo;
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

    std::vector<LogEntry> entradas_total = carrega_entradas(&total_leitura);
    std::vector<LogEntry> entradas_log = carrega_entradas(&log);

    if (entradas_log.empty()) {
      continue;
    }

    if (entradas_total.empty()) {
      std::ofstream total_escrita(caminho_total.c_str());
      for (std::size_t k = 0; k < entradas_log.size(); k++) {
        total_escrita << entradas_log[k].linha;
        if (k + 1 < entradas_log.size()) {
          total_escrita << "\n";
        }
      }
      continue;
    }

    std::vector<LogEntry> mesclado;
    mesclado.reserve(entradas_total.size() + entradas_log.size());

    std::size_t i = 0;
    std::size_t j = 0;
    while (i < entradas_total.size() && j < entradas_log.size()) {
      if (vem_antes(entradas_log[j], entradas_total[i])) {
        mesclado.push_back(entradas_log[j]);
        j++;
      } else {
        mesclado.push_back(entradas_total[i]);
        i++;
      }
    }
    while (i < entradas_total.size()) {
      mesclado.push_back(entradas_total[i]);
      i++;
    }
    while (j < entradas_log.size()) {
      mesclado.push_back(entradas_log[j]);
      j++;
    }

    std::ofstream total_escrita(caminho_total.c_str());
    for (std::size_t k = 0; k < mesclado.size(); k++) {
      total_escrita << mesclado[k].linha;
      if (k + 1 < mesclado.size()) {
        total_escrita << "\n";
      }
    }
  }

  return true;
}
