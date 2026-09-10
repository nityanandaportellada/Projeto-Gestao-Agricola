//Modulo de cabecalho responsavel por disponibilizar as funcoes de consulta e analise do historico dos talhoes
#ifndef HISTORICO_H
#define HISTORICO_H

#include <sqlite3.h>

//Declara as funcoes utilizadas para consultar o historico geral, o historico por talhao e a evolucao do risco
void listarHistorico(sqlite3 *db);
void historicoPorTalhao(sqlite3 *db);
void evolucaoTalhao(sqlite3 *db);

#endif