#ifndef MENU_H
#define MENU_H

#include <sqlite3.h>

void menuTalhoes(sqlite3 *db);
void menuPragas(sqlite3 *db);
void menuClima(sqlite3 *db);
void menuRelatorios(sqlite3 *db);
void menuHistorico(sqlite3 *db);

#endif