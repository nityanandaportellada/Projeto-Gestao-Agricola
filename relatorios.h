//Modulo de cabecalho responsavel por disponibilizar as funcoes de relatorios e as operacoes de leitura e escrita em arquivos CSV e TXT
#ifndef RELATORIOS_H
#define RELATORIOS_H

#include <sqlite3.h>

void gerarRelatorioClima(sqlite3 *db);
void gerarRelatorioPragasClima(sqlite3 *db);
void resumoPorPeriodo(sqlite3 *db);
//APS 2 - Declara a funcao responsavel pela exportacao dos dados do sistema para arquivos CSV
void exportarTalhoesCSV(sqlite3 *db);
//APS 2 - Declara a funcao responsavel pela leitura dos arquivos CSV gerados pelo sistema
void lerTalhoesCSV();
//APS 2 - Declara a funcao responsavel pela escrita do relatorio geral em arquivo TXT
void exportarRelatorioTXT(sqlite3 *db);

#endif