//Modulo de cabecalho responsavel por disponibilizar as funcoes dos submenus utilizados pelo sistema
#ifndef MENU_H
#define MENU_H

#include <sqlite3.h>

//Declara as funcoes responsaveis pelos diferentes submenus do sistema
void menuTalhoes(sqlite3 *db);
void menuPragas(sqlite3 *db);
void menuClima(sqlite3 *db);
void menuRelatorios(sqlite3 *db);
void menuHistorico(sqlite3 *db);

#endif