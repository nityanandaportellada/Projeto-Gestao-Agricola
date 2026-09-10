//header do banco de dados que será criado
#ifndef BANCO_H
#define BANCO_H

#include <sqlite3.h>

//Declara a funcao responsavel por inicializar e criar as tabelas utilizadas pelo sistema
int inicializarBanco(sqlite3 *db);

#endif