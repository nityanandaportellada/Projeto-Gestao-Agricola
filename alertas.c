#include <stdio.h>
#include <sqlite3.h>
#include "alertas.h"
#include "clima.h"

//Função para gerar alertas dos talhoes com risco alto ou muito alto

void gerarAlertas(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT codigo, nome "
        "FROM talhoes "
        "ORDER BY codigo;";

    //Prepara a consulta dos talhoes

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {

        printf("Erro ao consultar talhoes.\n");
        return;
    }

    printf("\n====================================\n");
    printf("              ALERTAS\n");
    printf("====================================\n");

    int encontrou = 0;

    //Percorre os talhoes cadastrados
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        int codigoTalhao = sqlite3_column_int(stmt, 0);
        const char *nomeTalhao = (const char *)sqlite3_column_text(stmt, 1);
        int riscoTemperatura = 1;
        int riscoUmidade = 1;
        int riscoPraga = 1;

        //Busca o ultimo registro climatico do talhao
        sqlite3_stmt *stmtClima;

        const char *sqlClima =
            "SELECT temperatura, umidade "
            "FROM clima "
            "WHERE codigo_talhao = ? "
            "ORDER BY id DESC "
            "LIMIT 1;";

        if (sqlite3_prepare_v2(db, sqlClima, -1, &stmtClima, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmtClima, 1, codigoTalhao);

            if (sqlite3_step(stmtClima) == SQLITE_ROW) {
                float temperatura = sqlite3_column_double(stmtClima, 0);
                float umidade = sqlite3_column_double(stmtClima, 1);
                riscoTemperatura = calcularRiscoTemperatura(temperatura);
                riscoUmidade = calcularRiscoUmidade(umidade);
            }
            sqlite3_finalize(stmtClima);
        }

        //Busca o maior risco de praga registrado no talhao
        sqlite3_stmt *stmtPraga;

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
            sqlite3_bind_int(stmtPraga, 1, codigoTalhao);
            sqlite3_bind_int(stmtPraga, 2, codigoTalhao);

            if (sqlite3_step(stmtPraga) == SQLITE_ROW) {

                if (sqlite3_column_type(stmtPraga, 0) != SQLITE_NULL) {
                    riscoPraga = sqlite3_column_int(stmtPraga, 0);
                }
            }

            sqlite3_finalize(stmtPraga);
        }

        //Calcula o risco final do talhao

        int riscoFinal = riscoTemperatura * riscoUmidade * riscoPraga;

        //Gera alerta somente para riscos altos e muito altos

        if (riscoFinal >= 17) {

            encontrou = 1;

            printf("\n-----------------------------\n");
            printf("Talhao: %d\n", codigoTalhao);
            printf("Nome: %s\n", nomeTalhao);
            printf("Risco temperatura: %d\n", riscoTemperatura);
            printf("Risco umidade: %d\n", riscoUmidade);
            printf("Risco praga: %d\n", riscoPraga);
            printf("Risco final: %d\n", riscoFinal);

            if (riscoFinal <= 36) {
                printf("Risco: ALTO\n");
                printf("ALERTA: Talhao apresenta risco alto!\n");

            } 
            
            else {
                printf("Risco: MUITO ALTO\n");
                printf("ALERTA CRITICO: Talhao apresenta risco muito alto!\n");
            }
        }
    }

    if (!encontrou) {
        printf("\nNenhum alerta encontrado.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}