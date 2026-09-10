#include <stdio.h>
#include <sqlite3.h>
#include "banco.h"

//Modulo que prepara, cria e constroi o banco de dados

// função para inicializar o banco de dados que usaremos para persistencia dos dados
int inicializarBanco(sqlite3 *db)
{
    //Variavel utilizada pelo SQLite para armazenar uma mensagem caso ocorra algum erro durante a execucao dos comandos SQL
    char *erro = NULL;

    //Conjunto de comandos SQL utilizados para configurar e criar as tabelas necessarias para o sistema
    const char *sql =

        //Ativa o uso de chaves estrangeiras no SQLite para garantir os relacionamentos entre as tabelas
        "PRAGMA foreign_keys = ON;"

        //Cria a tabela responsavel por armazenar os talhoes caso ela ainda nao exista no banco de dados
        "CREATE TABLE IF NOT EXISTS talhoes ("
        "codigo INTEGER PRIMARY KEY,"
        "nome TEXT NOT NULL,"
        "area REAL NOT NULL,"
        "plantacao TEXT NOT NULL,"
        "localizacao TEXT NOT NULL"
        ");"

        //Cria a tabela que armazena as pragas cadastradas e o nivel de risco associado a cada uma delas
        "CREATE TABLE IF NOT EXISTS pragas ("
        "codigo INTEGER PRIMARY KEY,"
        "nome TEXT NOT NULL,"
        "descricao TEXT NOT NULL,"
        "nivel_risco INTEGER NOT NULL"
        ");"

        //Cria a tabela responsavel por registrar as ocorrencias de pragas associadas aos talhoes cadastrados
        "CREATE TABLE IF NOT EXISTS ocorrencias_pragas ("
        "codigo INTEGER PRIMARY KEY,"
        "codigo_praga INTEGER NOT NULL,"
        "codigo_talhao INTEGER NOT NULL,"
        "nivel_infestacao INTEGER NOT NULL,"
        "area_afetada REAL NOT NULL,"
        "data TEXT NOT NULL,"

        //Relaciona cada ocorrencia com uma praga cadastrada
        "FOREIGN KEY (codigo_praga) REFERENCES pragas(codigo),"

        //Relaciona cada ocorrencia com um talhao cadastrado
        "FOREIGN KEY (codigo_talhao) REFERENCES talhoes(codigo)"
        ");"

        //Cria a tabela responsavel pelo armazenamento dos registros climaticos dos talhoes
        "CREATE TABLE IF NOT EXISTS clima ("

        //O identificador e gerado automaticamente pelo banco para cada novo registro climatico
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "codigo_talhao INTEGER NOT NULL,"
        "temperatura REAL NOT NULL,"
        "umidade REAL NOT NULL,"
        "data TEXT NOT NULL,"
        "hora TEXT NOT NULL,"

        //Relaciona cada registro climatico ao seu respectivo talhao
        "FOREIGN KEY (codigo_talhao) REFERENCES talhoes(codigo)"
        ");";


    //Executa todos os comandos SQL definidos anteriormente e verifica se ocorreu algum erro durante a inicializacao
    if (sqlite3_exec(db, sql, NULL, NULL, &erro) != SQLITE_OK) {

        printf("Erro ao criar as tabelas.\n");
        printf("Mensagem: %s\n", erro);

        //Libera a memoria utilizada pelo SQLite para armazenar a mensagem de erro
        sqlite3_free(erro);

        return 0;
    }

    printf("Banco de dados inicializado com sucesso.\n");

    //Retorna 1 indicando que a inicializacao ocorreu corretamente
    return 1;
}