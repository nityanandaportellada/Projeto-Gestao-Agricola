//Modulo responsavel pelos submenus do sistema e pelo direcionamento das opcoes escolhidas pelo usuario para as funcoes de cada modulo
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "menu.h"
#include "talhoes.h"
#include "pragas.h"
#include "clima.h"
#include "relatorios.h"
#include "historico.h"


//MENU DE TALHOES
  void menuTalhoes(sqlite3 *db) {
    int opcao;

    do {
        printf("\n====================================\n");
        printf("             TALHOES\n");
        printf("====================================\n");

        printf("1 - Cadastrar talhao\n");
        printf("2 - Listar talhoes\n");
        printf("3 - Editar talhao\n");
        printf("4 - Excluir talhao\n");
        printf("0 - Voltar\n");

        printf("\nEscolha uma opcao: ");

        //Valida se a opcao digitada pelo usuario e um numero inteiro
        if (scanf("%d", &opcao) != 1) {
            printf("\nDigite somente numeros.\n");

            //Limpa os caracteres restantes da entrada invalida
            while (getchar() != '\n');

            continue;
        }

        //Direciona a opcao escolhida para a funcao correspondente do modulo de talhoes
        switch(opcao) {

            case 1:
                cadastrarTalhao(db);
                break;

            case 2:
                listarTalhoes(db);
                break;

            case 3:
                editarTalhao(db);
                break;

            case 4:
                excluirTalhao(db);
                break;

            case 0:
                printf("\nVoltando ao menu principal...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while(opcao != 0);
}


//MENU DE PRAGAS
void menuPragas(sqlite3 *db) {
    int opcao;

    do {
        printf("\n====================================\n");
        printf("             PRAGAS\n");
        printf("====================================\n");

        printf("1 - Listar pragas\n");
        printf("2 - Cadastrar praga\n");
        printf("3 - Editar praga\n");
        printf("4 - Excluir praga\n");
        printf("5 - Cadastrar ocorrencia\n");
        printf("6 - Listar ocorrencias\n");
        printf("7 - Editar ocorrencia\n");
        printf("8 - Excluir ocorrencia\n");
        printf("0 - Voltar\n");

        printf("\nEscolha uma opcao: ");

        //Valida se a opcao digitada pelo usuario e um numero inteiro
        if (scanf("%d", &opcao) != 1) {
            printf("\nDigite somente numeros.\n");

            //Limpa os caracteres restantes da entrada invalida
            while (getchar() != '\n');

            continue;
        }

        //Direciona a opcao escolhida para as funcoes de cadastro e gerenciamento de pragas e ocorrencias
        switch(opcao) {

            case 1:
                listarPragas(db);
                break;

            case 2:
                cadastrarPraga(db);
                break;

            case 3:
                editarPraga(db);
                break;

            case 4:
                excluirPraga(db);
                break;

            case 5:
                cadastrarOcorrencia(db);
                break;

            case 6:
                listarOcorrencias(db);
                break;

            case 7:
                editarOcorrencia(db);
                break;

            case 8:
                excluirOcorrencia(db);
                break;

            case 0:
                printf("\nVoltando ao menu principal...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while(opcao != 0);
}


//MENU DE CLIMA
void menuClima(sqlite3 *db) {
    int opcao;

    do {
        printf("\n====================================\n");
        printf("             CLIMA\n");
        printf("====================================\n");

        printf("1 - Registrar dados climaticos\n");
        printf("2 - Listar historico climatico\n");
        printf("3 - Visualizar serie por talhao\n");
        printf("4 - Editar registro climatico\n");
        printf("5 - Excluir registro climatico\n");
        printf("0 - Voltar\n");

        printf("\nEscolha uma opcao: ");

        //Valida se a opcao digitada pelo usuario e um numero inteiro
        if (scanf("%d", &opcao) != 1) {
            printf("\nDigite somente numeros.\n");

            //Limpa os caracteres restantes da entrada invalida
            while (getchar() != '\n');

            continue;
        }

        //Direciona a opcao escolhida para a funcao correspondente do modulo de clima
        switch(opcao) {

            case 1:
                registrarClima(db);
                break;

            case 2:
                listarHistoricoClima(db);
                break;

            case 3:
                visualizarSerieTalhao(db);
                break;

            case 4:
                editarClima(db);
                break;

            case 5:
                excluirClima(db);
                break;

            case 0:
                printf("\nVoltando ao menu principal...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while(opcao != 0);
}

//MENU DE RELATORIOS
void menuRelatorios(sqlite3 *db) {
    int opcao;

    do {
        printf("\n====================================\n");
        printf("            RELATORIOS\n");
        printf("====================================\n");

        printf("1 - Relatorio de clima\n");
        printf("2 - Relatorio de pragas e clima\n");
        printf("3 - Resumo por periodo\n");
        printf("4 - Exportar Dados para CSV\n");
        printf("5 - Ler arquivo CSV\n");
        printf("6 - Exportar relatorio para TXT\n");
        printf("0 - Voltar\n");

        printf("\nEscolha uma opcao: ");

        //Valida se a opcao digitada pelo usuario e um numero inteiro
        if (scanf("%d", &opcao) != 1) {
            printf("\nDigite somente numeros.\n");

            //Limpa os caracteres restantes da entrada invalida
            while (getchar() != '\n');

            continue;
        }

        //Direciona a opcao escolhida para a geracao, exportacao ou leitura dos relatorios
        switch(opcao) {

            case 1:
                gerarRelatorioClima(db);
                break;

            case 2:
                gerarRelatorioPragasClima(db);
                break;

            case 3:
                resumoPorPeriodo(db);
                break;

            case 4:
                exportarTalhoesCSV(db);
                break;

            case 5:
                lerTalhoesCSV();
                break;

            case 6:
                exportarRelatorioTXT(db);
                break;

            case 0:
                printf("\nVoltando ao menu principal...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while(opcao != 0);
}

//menu do historico
void menuHistorico(sqlite3 *db)
{
    int opcao;

    do {
        printf("\n====================================\n");
        printf("             HISTORICO\n");
        printf("====================================\n");

        printf("1 - Historico geral\n");
        printf("2 - Historico por talhao\n");
        printf("3 - Evolucao do risco\n");
        printf("0 - Voltar\n");

        printf("\nEscolha uma opcao: ");

        //Valida se a opcao digitada pelo usuario e um numero inteiro
        if (scanf("%d", &opcao) != 1) {
            printf("\nDigite somente numeros.\n");

            //Limpa os caracteres restantes da entrada invalida
            while (getchar() != '\n');

            continue;
        }

        //Direciona a opcao escolhida para as diferentes consultas do historico
        switch(opcao) {

            case 1:
                listarHistorico(db);
                break;

            case 2:
                historicoPorTalhao(db);
                break;

            case 3:
                evolucaoTalhao(db);
                break;

            case 0:
                printf("\nVoltando ao menu principal...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while(opcao != 0);
}