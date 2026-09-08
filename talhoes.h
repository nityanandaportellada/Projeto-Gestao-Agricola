#ifndef TALHOES_H
#define TALHOES_H

#include <sqlite3.h>

typedef struct {
    int codigo;
    char nome[50];
    float area;
    char plantacao[50];
    char localizacao[100];
} Talhao;

void cadastrarTalhao(sqlite3 *db);
void listarTalhoes(sqlite3 *db);
int buscarTalhao(sqlite3 *db, int codigo);
void editarTalhao(sqlite3 *db);
void excluirTalhao(sqlite3 *db);

#endif