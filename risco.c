//Modulo responsavel por calcular e apresentar a classificacao de risco de cada talhao utilizando os fatores de clima e pragas
#include <stdio.h>
#include <sqlite3.h>
#include "clima.h"
#include "risco.h"
#include "talhoes.h"

//Percorre os talhoes cadastrados e calcula o risco final de cada um
void classificarRisco(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    //Consulta os talhoes cadastrados para realizar individualmente o calculo de risco
    const char *sql =
        "SELECT codigo, nome "
        "FROM talhoes "
        "ORDER BY codigo;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar talhoes.\n");
        return;
    }

    printf("\n====================================\n");
    printf("        CLASSIFICACAO DE RISCO\n");
    printf("====================================\n");

    //Controla se pelo menos um talhao foi encontrado durante a consulta
    int encontrou = 0;

    //Percorre todos os talhoes cadastrados para calcular seus respectivos riscos
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        int codigoTalhao = sqlite3_column_int(stmt, 0);
        const char *nomeTalhao = (const char *)sqlite3_column_text(stmt, 1);

        //Os fatores de risco iniciam em 1 para representar o menor valor quando nao existirem dados correspondentes
        int riscoTemperatura = 1;
        int riscoUmidade = 1;
        int riscoPraga = 1;

        sqlite3_stmt *stmtClima;

        //Busca o registro climatico mais recente do talhao utilizando o maior id cadastrado
        const char *sqlClima =
            "SELECT temperatura, umidade "
            "FROM clima "
            "WHERE codigo_talhao = ? "
            "ORDER BY id DESC "
            "LIMIT 1;";

        if (sqlite3_prepare_v2(db, sqlClima, -1, &stmtClima, NULL) == SQLITE_OK) {

            sqlite3_bind_int(stmtClima, 1, codigoTalhao);

            //Calcula os fatores de risco climaticos quando existir um registro para o talhao
            if (sqlite3_step(stmtClima) == SQLITE_ROW) {

                float temperatura = sqlite3_column_double(stmtClima, 0);
                float umidade = sqlite3_column_double(stmtClima, 1);
                riscoTemperatura = calcularRiscoTemperatura(temperatura);
                riscoUmidade = calcularRiscoUmidade(umidade);
            }

            sqlite3_finalize(stmtClima);
        }

        sqlite3_stmt *stmtPraga;

        //Busca o maior nivel de risco das pragas registradas na data de ocorrencia mais recente do talhao
        const char *sqlPraga =
            "SELECT MAX(p.nivel_risco) "
            "FROM ocorrencias_pragas o "
            "INNER JOIN pragas p "
            "ON o.codigo_praga = p.codigo "
            "WHERE o.codigo_talhao = ? "
            "AND o.data = ("
            "SELECT o2.data "
            "FROM ocorrencias_pragas o2 "
            "WHERE o2.codigo_talhao = ? "
            "ORDER BY "
            "substr(o2.data, 7, 4) || '-' || "
            "substr(o2.data, 4, 2) || '-' || "
            "substr(o2.data, 1, 2) DESC "
            "LIMIT 1"
            ");";

        if (sqlite3_prepare_v2(db, sqlPraga, -1, &stmtPraga, NULL) == SQLITE_OK) {

            //O mesmo codigo de talhao e associado aos dois parametros utilizados na consulta
            sqlite3_bind_int(stmtPraga, 1, codigoTalhao);
            sqlite3_bind_int(stmtPraga, 2, codigoTalhao);

            if (sqlite3_step(stmtPraga) == SQLITE_ROW) {

                //Mantem o valor padrao igual a 1 quando nenhuma ocorrencia de praga for encontrada
                if (sqlite3_column_type(stmtPraga, 0) != SQLITE_NULL) {
                    riscoPraga = sqlite3_column_int(stmtPraga, 0);
                }
            }

            sqlite3_finalize(stmtPraga);
        }

        //Calcula o risco final multiplicando os fatores de temperatura, umidade e praga
        int riscoFinal = riscoTemperatura * riscoUmidade * riscoPraga;

        printf("\n-----------------------------\n");
        printf("Talhao: %d\n", codigoTalhao);
        printf("Nome: %s\n", nomeTalhao);
        printf("Risco temperatura: %d\n", riscoTemperatura);
        printf("Risco umidade: %d\n", riscoUmidade);
        printf("Risco praga: %d\n", riscoPraga);
        printf("Risco final: %d\n", riscoFinal);

        //Classifica o resultado final nas faixas BAIXO, MEDIO, ALTO ou MUITO ALTO
        if (riscoFinal <= 6) { 
            printf("Classificacao: BAIXO\n");
        } 
        else if (riscoFinal <= 16) {
            printf("Classificacao: MEDIO\n");
        } 
        else if (riscoFinal <= 36) {
            printf("Classificacao: ALTO\n");
        } 
        else {
            printf("Classificacao: MUITO ALTO\n");
        }
    }

    //Informa quando nao existem talhoes cadastrados para realizar a classificacao
    if (!encontrou) {
        printf("\nNenhum talhao cadastrado.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}