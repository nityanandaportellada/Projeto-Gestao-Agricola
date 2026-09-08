#ifndef HISTORICO_H
#define HISTORICO_H

#include <sqlite3.h>

void listarHistorico(sqlite3 *db);
void historicoPorTalhao(sqlite3 *db);
void evolucaoTalhao(sqlite3 *db);

#endif