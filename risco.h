//Modulo de cabecalho responsavel por disponibilizar a funcao de classificacao de risco dos talhoes
#ifndef RISCO_H
#define RISCO_H

#include <sqlite3.h>

//Declara a funcao responsavel por calcular e apresentar a classificacao de risco dos talhoes
void classificarRisco(sqlite3 *db);

#endif