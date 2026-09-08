//header do banco de dados que será criado
#ifndef BANCO_H
#define BANCO_H

#include <sqlite3.h>

int inicializarBanco(sqlite3 *db);

#endif