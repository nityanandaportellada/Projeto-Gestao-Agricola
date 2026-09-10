#ifndef ALERTAS_H
#define ALERTAS_H

#include <sqlite3.h>

//Declara a funcao responsavel por analisar os riscos e gerar os alertas dos talhoes
void gerarAlertas(sqlite3 *db);

#endif