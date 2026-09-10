//Modulo principal responsavel por iniciar o sistema, abrir o banco de dados, apresentar o menu principal e direcionar o usuario para os demais modulos
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "banco.h"
#include "menu.h"
#include "alertas.h"
#include "risco.h"
#include "historico.h"
#include "visualizacao.h"


// MENU PRINCIPAL

int main()
{
    sqlite3 *db;
    int resultado;
    int opcao;


    // Para abrir o Banco

    resultado = sqlite3_open("agricola.db", &db);

    //Verifica se o banco de dados foi aberto corretamente antes de continuar a execucao do sistema
    if (resultado != SQLITE_OK) {
        printf("\nErro ao abrir o banco de dados.\n");
        sqlite3_close(db);
        return 1;
    }

    printf("\n====================================\n");
    printf("     SISTEMA DE GESTAO AGRICOLA\n");
    printf("====================================\n");

    // Para inicializar o banco
    if (!inicializarBanco(db)) {
        printf("\nNao foi possivel inicializar o banco.\n");
        sqlite3_close(db);
        return 1;
    }


    // Menu Principal do Programa
    do {
        printf("\n====================================\n");
        printf("          MENU PRINCIPAL\n");
        printf("====================================\n");

        printf("1 - Talhoes\n");
        printf("2 - Pragas e Ocorrencias\n");
        printf("3 - Clima\n");
        printf("4 - Classificacao de Risco\n");
        printf("5 - Relatorios\n");
        printf("6 - Historico\n");
        printf("7 - Alertas\n");
        printf("8 - Visualizacao da Fazenda\n");
        printf("0 - Sair\n");

        printf("\nEscolha uma opcao: ");

        //Valida se a opcao informada pelo usuario e um numero antes de processar o menu
        if (scanf("%d", &opcao) != 1) {
            printf("\nDigite somente numeros.\n");

            //Limpa os caracteres restantes da entrada invalida antes de apresentar novamente o menu
            while (getchar() != '\n');

            continue;
        }

        //Direciona a opcao selecionada para a funcao ou submenu correspondente
        switch (opcao) {

            case 1:
                menuTalhoes(db);
                break;

            case 2:
                menuPragas(db);
                break;

            case 3:
                menuClima(db);
                break;

            case 4:
                classificarRisco(db);                
                break;

            case 5:
                menuRelatorios(db);
                break;

            case 6:
                menuHistorico(db);
                break;

            case 7:
                gerarAlertas(db);
                break;

            case 8:
                visualizarFazenda(db);
                break;

            case 0:
                printf("\nEncerrando o sistema...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
                break;
        }

    } while (opcao != 0);

    // Para fechar o Banco

    sqlite3_close(db);

    printf("\nBanco de dados fechado.\n");
    printf("Sistema encerrado.\n");

    return 0;
}