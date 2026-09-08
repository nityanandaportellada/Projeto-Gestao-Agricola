#ifndef RELATORIOS_H
#define RELATORIOS_H

#include <sqlite3.h>

void gerarRelatorioClima(sqlite3 *db);
void gerarRelatorioPragasClima(sqlite3 *db);
void resumoPorPeriodo(sqlite3 *db);
void exportarTalhoesCSV(sqlite3 *db);
void lerTalhoesCSV();
void exportarRelatorioTXT(sqlite3 *db);

#endif