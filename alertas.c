#include <stdio.h>
#include <sqlite3.h>
#include "alertas.h"
#include "clima.h"

//Modulo responsavel pela geracao de alertas para talhoes que apresentem nivel de risco alto ou muito alto

//Função para gerar alertas dos talhoes com risco alto ou muito alto
void gerarAlertas(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    //Consulta os codigos e nomes de todos os talhoes cadastrados
    //para que o risco de cada um possa ser analisado
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

    //Variavel utilizada para identificar se pelo menos um alerta foi gerado
    int encontrou = 0;

    //Percorre os talhoes cadastrados
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        //Recupera o codigo e o nome do talhao atual
        int codigoTalhao = sqlite3_column_int(stmt, 0);
        const char *nomeTalhao = (const char *)sqlite3_column_text(stmt, 1);

        //Os riscos iniciam com valor 1, que representa o menor risco
        //caso nao existam registros climaticos ou ocorrencias de pragas
        int riscoTemperatura = 1;
        int riscoUmidade = 1;
        int riscoPraga = 1;

        //Busca o ultimo registro climatico do talhao
        sqlite3_stmt *stmtClima;

        //Seleciona temperatura e umidade do registro climatico mais recente
        //utilizando o ID em ordem decrescente e limitando o resultado a um registro
        const char *sqlClima =
            "SELECT temperatura, umidade "
            "FROM clima "
            "WHERE codigo_talhao = ? "
            "ORDER BY id DESC "
            "LIMIT 1;";

        //Prepara a consulta e associa o codigo do talhao ao parametro da consulta
        if (sqlite3_prepare_v2(db, sqlClima, -1, &stmtClima, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmtClima, 1, codigoTalhao);

            //Caso exista registro climatico, recupera temperatura e umidade
            //e calcula o risco correspondente a cada uma
            if (sqlite3_step(stmtClima) == SQLITE_ROW) {
                float temperatura = sqlite3_column_double(stmtClima, 0);
                float umidade = sqlite3_column_double(stmtClima, 1);
                riscoTemperatura = calcularRiscoTemperatura(temperatura);
                riscoUmidade = calcularRiscoUmidade(umidade);
            }

            //Finaliza a consulta dos dados climaticos
            sqlite3_finalize(stmtClima);
        }

        //Busca o maior risco de praga registrado no talhao
        sqlite3_stmt *stmtPraga;

        //Localiza a data mais recente com ocorrencia de praga no talhao
        //e retorna o maior nivel de risco das pragas registradas nessa data
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

        //Prepara a consulta e associa o codigo do talhao
        //aos dois parametros utilizados no comando SQL
        if (sqlite3_prepare_v2(db, sqlPraga, -1, &stmtPraga, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmtPraga, 1, codigoTalhao);
            sqlite3_bind_int(stmtPraga, 2, codigoTalhao);

            if (sqlite3_step(stmtPraga) == SQLITE_ROW) {

                //Verifica se foi encontrado algum risco de praga antes de alterar
                //o valor inicial definido para a variavel
                if (sqlite3_column_type(stmtPraga, 0) != SQLITE_NULL) {
                    riscoPraga = sqlite3_column_int(stmtPraga, 0);
                }
            }

            //Finaliza a consulta das ocorrencias de pragas
            sqlite3_finalize(stmtPraga);
        }

        //Calcula o risco final do talhao

        //Combina os tres fatores de risco por meio da multiplicacao
        int riscoFinal = riscoTemperatura * riscoUmidade * riscoPraga;

        //Gera alerta somente para riscos altos e muito altos

        if (riscoFinal >= 17) {

            //Registra que pelo menos um alerta foi encontrado
            encontrou = 1;

            printf("\n-----------------------------\n");
            printf("Talhao: %d\n", codigoTalhao);
            printf("Nome: %s\n", nomeTalhao);
            printf("Risco temperatura: %d\n", riscoTemperatura);
            printf("Risco umidade: %d\n", riscoUmidade);
            printf("Risco praga: %d\n", riscoPraga);
            printf("Risco final: %d\n", riscoFinal);

            //Classifica valores de 17 ate 36 como risco alto
            if (riscoFinal <= 36) {
                printf("Risco: ALTO\n");
                printf("ALERTA: Talhao apresenta risco alto!\n");

            } 
            
            //Valores acima de 36 sao classificados como risco muito alto
            else {
                printf("Risco: MUITO ALTO\n");
                printf("ALERTA CRITICO: Talhao apresenta risco muito alto!\n");
            }
        }
    }

    //Caso nenhum talhao tenha atingido risco alto ou muito alto
    //informa que nao existem alertas
    if (!encontrou) {
        printf("\nNenhum alerta encontrado.\n");
    }

    printf("-----------------------------\n");

    //Finaliza a consulta principal dos talhoes
    sqlite3_finalize(stmt);
}