// Copyright 2026 Carlos
#ifndef MONITORAMENTO_LOGS_MONITORA_LOGS_HPP_
#define MONITORAMENTO_LOGS_MONITORA_LOGS_HPP_

/**
 * @brief Processa a lista de arquivos de log e atualiza os arquivos total_*.
 * @param caminho_lista Caminho do arquivo com a lista de logs.
 * @return true se o processamento ocorreu sem erros fatais; false caso a lista
 *         nao possa ser aberta.
 * @pre caminho_lista != nullptr
 */
bool processa_lista_logs(char const * caminho_lista);

#endif  // MONITORAMENTO_LOGS_MONITORA_LOGS_HPP_
