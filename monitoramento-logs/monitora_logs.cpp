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

/**
 * @brief Normaliza separadores de diretorio para '/'.
 * @param caminho Caminho original.
 * @return Caminho com barras normalizadas.
 * @pre caminho nao pode ser vazio.
 */
std::string normaliza_caminho(std::string const& caminho) {
  assert(!caminho.empty());
  std::string saida = caminho;
  for (std::size_t i = 0; i < saida.size(); i++) {
    if (saida[i] == '\\') {
      saida[i] = '/';
    }
  }
  return saida;
}

/**
 * @brief Monta o caminho do arquivo total_ a partir do basename.
 * @param caminho_log Caminho do arquivo de log original.
 * @return Caminho do arquivo total_ baseado no basename.
 * @pre caminho_log nao pode ser vazio.
 */
std::string monta_caminho_total(std::string const& caminho_log) {
  assert(!caminho_log.empty());
  std::string::size_type pos = caminho_log.find_last_of("/\\");
  if (pos == std::string::npos) {
    return "total_" + caminho_log;
  }
  std::string base = caminho_log.substr(pos + 1);
  return "total_" + base;
}

/**
 * @brief Faz o parse de uma linha de log para LogEntry.
 * @param linha Linha original do log.
 * @param saida Estrutura preenchida em caso de sucesso.
 * @return true se o parse foi bem sucedido; false caso contrario.
 * @pre saida != nullptr
 */
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

/**
 * @brief Carrega entradas validas de um arquivo de log.
 * @param arquivo Stream aberto para leitura.
 * @return Vetor com entradas validas.
 * @pre arquivo != nullptr
 */
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

/**
 * @brief Compara duas entradas por timestamp.
 * @param esquerda Entrada da esquerda.
 * @param direita Entrada da direita.
 * @return true se esquerda vem antes de direita.
 */
bool vem_antes(LogEntry const& esquerda, LogEntry const& direita) {
  if (esquerda.ano != direita.ano) return esquerda.ano < direita.ano;
  if (esquerda.mes != direita.mes) return esquerda.mes < direita.mes;
  if (esquerda.dia != direita.dia) return esquerda.dia < direita.dia;
  if (esquerda.hora != direita.hora) return esquerda.hora < direita.hora;
  if (esquerda.minuto != direita.minuto) return esquerda.minuto < direita.minuto;
  return esquerda.segundo < direita.segundo;
}

/**
 * @brief Copia linhas de origem para destino.
 * @param origem Stream de origem.
 * @param destino Stream de destino.
 * @return true se pelo menos uma linha foi copiada.
 * @pre origem != nullptr
 * @pre destino != nullptr
 */
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

/**
 * @brief Escreve entradas no arquivo destino.
 * @param entradas Vetor de entradas.
 * @param destino Stream aberto para escrita.
 * @pre destino != nullptr
 */
void escreve_entradas(std::vector<LogEntry> const& entradas,
                      std::ofstream* destino) {
  assert(destino != nullptr);

  for (std::size_t k = 0; k < entradas.size(); k++) {
    *destino << entradas[k].linha;
    if (k + 1 < entradas.size()) {
      *destino << "\n";
    }
  }
}

/**
 * @brief Mescla entradas ordenadas mantendo estabilidade.
 * @param entradas_total Entradas do total existente.
 * @param entradas_log Entradas do log novo.
 * @return Vetor mesclado em ordem cronologica.
 */
std::vector<LogEntry> mescla_entradas(
    std::vector<LogEntry> const& entradas_total,
    std::vector<LogEntry> const& entradas_log) {
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

  return mesclado;
}

/**
 * @brief Processa um arquivo de log e atualiza o total_ correspondente.
 * @param caminho_log Caminho do arquivo de log (normalizado).
 * @return true se o log foi processado; false se nao foi possivel abrir.
 */
bool processa_log(std::string const& caminho_log) {
  std::ifstream log(caminho_log.c_str());
  if (!log.is_open()) {
    return false;
  }

  std::string caminho_total = monta_caminho_total(caminho_log);
  std::ifstream total_leitura(caminho_total.c_str());
  if (!total_leitura.is_open()) {
    std::vector<LogEntry> entradas_log = carrega_entradas(&log);
    std::ofstream total_escrita(caminho_total.c_str());
    escreve_entradas(entradas_log, &total_escrita);
    return true;
  }

  std::vector<LogEntry> entradas_total = carrega_entradas(&total_leitura);
  std::vector<LogEntry> entradas_log = carrega_entradas(&log);

  if (entradas_log.empty()) {
    return true;
  }

  if (entradas_total.empty()) {
    std::ofstream total_escrita(caminho_total.c_str());
    escreve_entradas(entradas_log, &total_escrita);
    return true;
  }

  std::vector<LogEntry> mesclado = mescla_entradas(entradas_total, entradas_log);
  std::ofstream total_escrita(caminho_total.c_str());
  escreve_entradas(mesclado, &total_escrita);
  return true;
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
    std::string caminho_normalizado = normaliza_caminho(caminho_log);
    processa_log(caminho_normalizado);
  }

  return true;
}
