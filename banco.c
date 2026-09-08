#include <stdio.h>
#include <sqlite3.h>

#include "banco.h"


// função para inicializar o banco de dados que usaremos para persistencia dos dados
int inicializarBanco(sqlite3 *db)
{
    char *erro = NULL;

    const char *sql =
        "PRAGMA foreign_keys = ON;"

        "CREATE TABLE IF NOT EXISTS talhoes ("
        "codigo INTEGER PRIMARY KEY,"
        "nome TEXT NOT NULL,"
        "area REAL NOT NULL,"
        "plantacao TEXT NOT NULL,"
        "localizacao TEXT NOT NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS pragas ("
        "codigo INTEGER PRIMARY KEY,"
        "nome TEXT NOT NULL,"
        "descricao TEXT NOT NULL,"
        "nivel_risco INTEGER NOT NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS ocorrencias_pragas ("
        "codigo INTEGER PRIMARY KEY,"
        "codigo_praga INTEGER NOT NULL,"
        "codigo_talhao INTEGER NOT NULL,"
        "nivel_infestacao INTEGER NOT NULL,"
        "area_afetada REAL NOT NULL,"
        "data TEXT NOT NULL,"
        "FOREIGN KEY (codigo_praga) REFERENCES pragas(codigo),"
        "FOREIGN KEY (codigo_talhao) REFERENCES talhoes(codigo)"
        ");"

        "CREATE TABLE IF NOT EXISTS clima ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "codigo_talhao INTEGER NOT NULL,"
        "temperatura REAL NOT NULL,"
        "umidade REAL NOT NULL,"
        "data TEXT NOT NULL,"
        "hora TEXT NOT NULL,"
        "FOREIGN KEY (codigo_talhao) REFERENCES talhoes(codigo)"
        ");";


    if (sqlite3_exec(db, sql, NULL, NULL, &erro) != SQLITE_OK) {

        printf("Erro ao criar as tabelas.\n");
        printf("Mensagem: %s\n", erro);

        sqlite3_free(erro);

        return 0;
    }

    printf("Banco de dados inicializado com sucesso.\n");

    return 1;
}