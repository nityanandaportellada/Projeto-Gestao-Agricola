//Modulo de cabecalho responsavel por definir a estrutura Talhao e disponibilizar as funcoes de gerenciamento dos talhoes
#ifndef TALHOES_H
#define TALHOES_H

#include <sqlite3.h>

//Estrutura utilizada para representar os dados de um talhao
typedef struct {
    int codigo;
    char nome[50];
    float area;
    char plantacao[50];
    char localizacao[100];
} Talhao;

//Declara as funcoes utilizadas para cadastrar, listar, buscar, editar e excluir talhoes
void cadastrarTalhao(sqlite3 *db);
void listarTalhoes(sqlite3 *db);
int buscarTalhao(sqlite3 *db, int codigo);
void editarTalhao(sqlite3 *db);
void excluirTalhao(sqlite3 *db);

#endif