#include <stdio.h>
#include <sqlite3.h>
#include "historico.h"
#include "clima.h"
#include "talhoes.h"

//Função para listar o historico geral por talhao

void listarHistorico(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT o.codigo, "
        "o.codigo_talhao, "
        "t.nome, "
        "p.nome, "
        "o.nivel_infestacao, "
        "o.area_afetada, "
        "o.data "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "INNER JOIN talhoes t "
        "ON o.codigo_talhao = t.codigo "
        "ORDER BY o.codigo_talhao, "
        "substr(o.data, 7, 4) || '-' || "
        "substr(o.data, 4, 2) || '-' || "
        "substr(o.data, 1, 2);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar historico.\n");
        return;
    }

    printf("\n====================================\n");
    printf("          HISTORICO GERAL\n");
    printf("====================================\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        int nivel = sqlite3_column_int(stmt, 4);

        printf("\n-----------------------------\n");
        printf("Tipo: Registro de Ocorrencia\n");
        printf("Codigo da ocorrencia: %d\n", sqlite3_column_int(stmt, 0));
        printf("Talhao: %d\n", sqlite3_column_int(stmt, 1));
        printf("Nome do talhao: %s\n", (const char *)sqlite3_column_text(stmt, 2));
        printf("Praga: %s\n", (const char *)sqlite3_column_text(stmt, 3));
        printf("Nivel de infestacao: %d\n", nivel);
        printf("Area afetada: %.2f hectares\n", sqlite3_column_double(stmt, 5));
        printf("Data: %s\n", (const char *)sqlite3_column_text(stmt, 6));
    }

    if (encontrou == 0) {
        printf("\nNenhum registro encontrado.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}

//Função para listar o historico de um talhao

void historicoPorTalhao(sqlite3 *db)
{
    sqlite3_stmt *stmt;
    int codigoTalhao;

    printf("\nDigite o codigo do talhao: ");

    if (scanf("%d", &codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    if (buscarTalhao(db, codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    const char *sql =
        "SELECT o.codigo, "
        "p.nome, "
        "o.nivel_infestacao, "
        "o.area_afetada, "
        "o.data "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "WHERE o.codigo_talhao = ? "
        "ORDER BY "
        "substr(o.data, 7, 4) || '-' || "
        "substr(o.data, 4, 2) || '-' || "
        "substr(o.data, 1, 2);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar historico por talhao.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigoTalhao);

    printf("\n====================================\n");
    printf("       HISTORICO DO TALHAO %d\n", codigoTalhao);
    printf("====================================\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        int nivel = sqlite3_column_int(stmt, 2);

        printf("\n-----------------------------\n");
        printf("Codigo da ocorrencia: %d\n", sqlite3_column_int(stmt, 0));
        printf("Praga: %s\n", (const char *)sqlite3_column_text(stmt, 1));
        printf("Nivel de infestacao: %d\n", nivel);
        printf("Area afetada: %.2f hectares\n", sqlite3_column_double(stmt, 3));
        printf("Data: %s\n", (const char *)sqlite3_column_text(stmt, 4));
    }

    if (encontrou == 0) {
        printf("\nNenhum registro encontrado para esse talhao.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}

void evolucaoTalhao(sqlite3 *db)
{
    sqlite3_stmt *stmt;
    int codigoTalhao;
    int riscoAnterior = -1;

    printf("\nDigite o codigo do talhao: ");

    if (scanf("%d", &codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    if (buscarTalhao(db, codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    const char *sql =
        "SELECT o.data, "
        "MAX(p.nivel_risco), "
        "c.temperatura, "
        "c.umidade "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "LEFT JOIN clima c "
        "ON c.id = ("
        "SELECT MAX(c2.id) "
        "FROM clima c2 "
        "WHERE c2.codigo_talhao = o.codigo_talhao "
        "AND c2.data = o.data"
        ") "
        "WHERE o.codigo_talhao = ? "
        "GROUP BY o.data "
        "ORDER BY "
        "substr(o.data, 7, 4) || '-' || "
        "substr(o.data, 4, 2) || '-' || "
        "substr(o.data, 1, 2);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar evolucao do talhao.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigoTalhao);

    printf("\n====================================\n");
    printf("       EVOLUCAO DO TALHAO %d\n", codigoTalhao);
    printf("====================================\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        const char *data =
            (const char *)sqlite3_column_text(stmt, 0);

        int riscoPraga = sqlite3_column_int(stmt, 1);

        int riscoTemperatura = 1;
        int riscoUmidade = 1;

        printf("\n-----------------------------\n");
        printf("Data: %s\n", data);

        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL &&
            sqlite3_column_type(stmt, 3) != SQLITE_NULL) {

            float temperatura = sqlite3_column_double(stmt, 2);
            float umidade = sqlite3_column_double(stmt, 3);

            riscoTemperatura =
                calcularRiscoTemperatura(temperatura);

            riscoUmidade =
                calcularRiscoUmidade(umidade);

            printf("Temperatura: %.1f C\n", temperatura);
            printf("Umidade: %.1f%%\n", umidade);
        }
        else {
            printf("Nao existem dados climaticos para esta data.\n");
        }

        printf("Risco temperatura: %d\n", riscoTemperatura);
        printf("Risco umidade: %d\n", riscoUmidade);
        printf("Risco praga: %d\n", riscoPraga);

        int riscoFinal =
            riscoTemperatura * riscoUmidade * riscoPraga;

        printf("Risco final: %d\n", riscoFinal);

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

        if (riscoAnterior != -1) {

            if (riscoFinal > riscoAnterior) {
                printf("Situacao: risco aumentou\n");
            }
            else if (riscoFinal < riscoAnterior) {
                printf("Situacao: risco diminuiu\n");
            }
            else {
                printf("Situacao: risco permaneceu estavel\n");
            }
        }
        else {
            printf("Situacao: primeiro registro de risco\n");
        }

        riscoAnterior = riscoFinal;
    }

    if (encontrou == 0) {
        printf("\nNenhuma ocorrencia encontrada para esse talhao.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}