#ifndef PRAGAS_H
#define PRAGAS_H
#include <sqlite3.h>

//estrutura da praga
typedef struct {
    int codigo;
    char nome[50];
    char descricao[200];
    int nivelRisco;
} Praga;

//estrutura da ocorrência de praga
typedef struct {
    int codigo;
    int codigoPraga;
    int codigoTalhao;
    int nivelInfestacao;
    float areaAfetada;
    char data[20];
} OcorrenciaPraga;

//Função para listar as pragas cadastradas
void listarPragas(sqlite3 *db);

//Função para cadastrar uma praga
void cadastrarPraga(sqlite3 *db);

//Função que busca uma praga cadastrada
int buscarPraga(sqlite3 *db, int codigo);

//Função para cadastrar uma ocorrência
void cadastrarOcorrencia(sqlite3 *db);

//Função para listar as ocorrências
void listarOcorrencias(sqlite3 *db);

//Função que busca uma ocorrência
int buscarOcorrencia(sqlite3 *db, int codigo);

//Edita uma ocorrência cadastrada
void editarOcorrencia(sqlite3 *db);

//Exclui uma ocorrência cadastrada
void excluirOcorrencia(sqlite3 *db);

//Edita uma praga cadastrada
void editarPraga(sqlite3 *db);

//exclui uma praga cadastrada
void excluirPraga(sqlite3 *db);
#endif