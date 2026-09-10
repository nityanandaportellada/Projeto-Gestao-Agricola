//Modulo responsavel pela visualizacao grafica dos talhoes da fazenda utilizando Raylib e pela representacao visual dos seus niveis de risco
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <raylib.h>

#include "visualizacao.h"
#include "clima.h"

//Calcula o risco atual de um talhao para a visualizacao
int calcularRiscoVisualizacao(sqlite3 *db, int codigoTalhao)
{
    //Os fatores de risco iniciam em 1 para representar o menor valor quando nao existirem dados correspondentes
    int riscoTemperatura = 1;
    int riscoUmidade = 1;
    int riscoPraga = 1;
    int riscoFinal;

    float temperatura;
    float umidade;

    sqlite3_stmt *stmt;

    //Busca o registro climatico mais recente do talhao utilizando o maior id cadastrado
    const char *sqlClima =
        "SELECT temperatura, umidade "
        "FROM clima "
        "WHERE codigo_talhao = ? "
        "ORDER BY id DESC "
        "LIMIT 1;";

    if (sqlite3_prepare_v2(db, sqlClima, -1, &stmt, NULL) == SQLITE_OK) {

        sqlite3_bind_int(stmt, 1, codigoTalhao);

        //Calcula os fatores de risco de temperatura e umidade quando existirem dados climaticos para o talhao
        if (sqlite3_step(stmt) == SQLITE_ROW) {

            temperatura = sqlite3_column_double(stmt, 0);
            umidade = sqlite3_column_double(stmt, 1);

            riscoTemperatura = calcularRiscoTemperatura(temperatura);
            riscoUmidade = calcularRiscoUmidade(umidade);
        }

        sqlite3_finalize(stmt);
    }

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

    if (sqlite3_prepare_v2(db, sqlPraga, -1, &stmt, NULL) == SQLITE_OK) {

        //Associa o mesmo codigo de talhao aos dois parametros utilizados pela consulta
        sqlite3_bind_int(stmt, 1, codigoTalhao);
        sqlite3_bind_int(stmt, 2, codigoTalhao);

        if (sqlite3_step(stmt) == SQLITE_ROW) {

            //Mantem o risco de praga igual a 1 quando nenhuma ocorrencia for encontrada
            if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
                riscoPraga = sqlite3_column_int(stmt, 0);
            }
        }

        sqlite3_finalize(stmt);
    }

    //Calcula o risco final multiplicando os fatores de temperatura, umidade e praga
    riscoFinal = riscoTemperatura * riscoUmidade * riscoPraga;

    return riscoFinal;
}

//Define a cor do talhao de acordo com o risco
Color definirCorRisco(int risco)
{
    if (risco <= 6) {
        return GREEN;
    }
    else if (risco <= 16) {
        return YELLOW;
    }
    else if (risco <= 36) {
        return ORANGE;
    }
    else {
        return RED;
    }
}

//Retorna o texto correspondente ao nivel de risco
const char *textoRisco(int risco)
{
    if (risco <= 6) {
        return "BAIXO";
    }
    else if (risco <= 16) {
        return "MEDIO";
    }
    else if (risco <= 36) {
        return "ALTO";
    }
    else {
        return "MUITO ALTO";
    }
}

//Exibe os talhoes cadastrados em uma visao da fazenda
void visualizarFazenda(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    int quantidade = 0;

    //Consulta quantos talhoes existem para definir se a visualizacao pode ser aberta
    const char *sqlQuantidade =
        "SELECT COUNT(*) FROM talhoes;";

    if (sqlite3_prepare_v2(db, sqlQuantidade, -1, &stmt, NULL) != SQLITE_OK) {
        printf("\nErro ao consultar talhoes.\n");
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        quantidade = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    //Impede a abertura da janela grafica quando nenhum talhao estiver cadastrado
    if (quantidade == 0) {
        printf("\nNenhum talhao cadastrado.\n");
        printf("Cadastre um talhao antes de visualizar a fazenda.\n");
        return;
    }

    //Define as dimensoes da janela utilizada pela visualizacao da fazenda
    const int larguraTela = 1200;
    const int alturaTela = 750;

    InitWindow(larguraTela, alturaTela, "Visualizacao da Fazenda");

    //Limita a atualizacao da janela grafica a 60 quadros por segundo
    SetTargetFPS(60);

    //Mantem a janela em execucao ate que o usuario solicite seu fechamento
    while (!WindowShouldClose()) {

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("VISUALIZACAO DA FAZENDA", 390, 20, 28, DARKGREEN);

        DrawText("Verde: Baixo", 30, 65, 16, GREEN);
        DrawText("Amarelo: Medio", 170, 65, 16, GOLD);
        DrawText("Laranja: Alto", 330, 65, 16, ORANGE);
        DrawText("Vermelho: Muito Alto", 470, 65, 16, RED);

        int colunas;

        //Define dinamicamente a quantidade de colunas de acordo com o numero total de talhoes cadastrados
        if (quantidade <= 4) {
            colunas = 2;
        }
        else if (quantidade <= 9) {
            colunas = 3;
        }
        else {
            colunas = 4;
        }

        //Calcula a quantidade necessaria de linhas para distribuir todos os talhoes na tela
        int linhas = (quantidade + colunas - 1) / colunas;

        int margemX = 30;
        int inicioY = 105;
        int margemInferior = 45;
        int espaco = 15;

        //Calcula o espaco horizontal disponivel para distribuir os blocos dos talhoes
        int larguraDisponivel =
            larguraTela - (margemX * 2);

        //Calcula o espaco vertical disponivel descontando titulo, legenda e margem inferior
        int alturaDisponivel =
            alturaTela - inicioY - margemInferior;

        //Calcula automaticamente a largura de cada bloco conforme a quantidade de colunas
        int largura =
            (larguraDisponivel - ((colunas - 1) * espaco)) / colunas;

        //Calcula automaticamente a altura de cada bloco conforme a quantidade de linhas
        int altura =
            (alturaDisponivel - ((linhas - 1) * espaco)) / linhas;

        int tamanhoFonte = 16;
        int tamanhoTitulo = 20;

        //Reduz o tamanho das fontes quando a quantidade de linhas aumenta para manter as informacoes dentro dos blocos
        if (linhas >= 4) {
            tamanhoFonte = 13;
            tamanhoTitulo = 17;
        }

        if (linhas >= 5) {
            tamanhoFonte = 11;
            tamanhoTitulo = 15;
        }

        //Consulta os dados necessarios para representar cada talhao na visualizacao
        const char *sql =
            "SELECT codigo, nome, area, plantacao, localizacao "
            "FROM talhoes "
            "ORDER BY codigo;";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {

            //Controla a posicao sequencial de cada talhao dentro da grade da visualizacao
            int posicao = 0;

            while (sqlite3_step(stmt) == SQLITE_ROW) {

                int codigo = sqlite3_column_int(stmt, 0);

                const char *nome =
                    (const char *)sqlite3_column_text(stmt, 1);

                float area =
                    sqlite3_column_double(stmt, 2);

                const char *plantacao =
                    (const char *)sqlite3_column_text(stmt, 3);

                const char *localizacao =
                    (const char *)sqlite3_column_text(stmt, 4);

                //Calcula em qual coluna e linha da grade o talhao atual sera exibido
                int coluna = posicao % colunas;
                int linha = posicao / colunas;

                //Calcula a coordenada horizontal do bloco a partir da coluna
                int x =
                    margemX + coluna * (largura + espaco);

                //Calcula a coordenada vertical do bloco a partir da linha
                int y =
                    inicioY + linha * (altura + espaco);

                //Calcula o risco atual do talhao antes de definir sua representacao visual
                int risco =
                    calcularRiscoVisualizacao(db, codigo);

                //Converte o nivel de risco calculado na cor correspondente
                Color cor =
                    definirCorRisco(risco);

                //Desenha o fundo do bloco utilizando a cor correspondente ao risco com transparencia
                DrawRectangle(
                    x,
                    y,
                    largura,
                    altura,
                    Fade(cor, 0.35f)
                );

                //Desenha o contorno do bloco que representa o talhao
                DrawRectangleLines(
                    x,
                    y,
                    largura,
                    altura,
                    DARKGRAY
                );

                char texto[200];

                int textoX = x + 12;
                int textoY = y + 10;

                //Monta e exibe o codigo do talhao dentro do bloco
                sprintf(texto, "Talhao %d", codigo);
                DrawText(
                    texto,
                    textoX,
                    textoY,
                    tamanhoTitulo,
                    BLACK
                );

                textoY += tamanhoTitulo + 8;

                //Monta e exibe o nome do talhao
                sprintf(texto, "Nome: %s", nome);
                DrawText(
                    texto,
                    textoX,
                    textoY,
                    tamanhoFonte,
                    BLACK
                );

                textoY += tamanhoFonte + 7;

                //Monta e exibe o tipo de plantacao existente no talhao
                sprintf(texto, "Plantacao: %s", plantacao);
                DrawText(
                    texto,
                    textoX,
                    textoY,
                    tamanhoFonte,
                    BLACK
                );

                textoY += tamanhoFonte + 7;

                //Monta e exibe a area cadastrada para o talhao
                sprintf(texto, "Area: %.2f ha", area);
                DrawText(
                    texto,
                    textoX,
                    textoY,
                    tamanhoFonte,
                    BLACK
                );

                textoY += tamanhoFonte + 7;

                //Monta e exibe a localizacao cadastrada para o talhao
                sprintf(texto, "Local: %s", localizacao);
                DrawText(
                    texto,
                    textoX,
                    textoY,
                    tamanhoFonte,
                    BLACK
                );

                textoY += tamanhoFonte + 7;

                //Monta o texto contendo a classificacao e o valor numerico do risco calculado
                sprintf(
                    texto,
                    "Risco: %s (%d)",
                    textoRisco(risco),
                    risco
                );

                DrawText(
                    texto,
                    textoX,
                    textoY,
                    tamanhoFonte,
                    BLACK
                );

                //Avanca a posicao utilizada para calcular o proximo bloco da grade
                posicao++;
            }

            sqlite3_finalize(stmt);
        }

        DrawText(
            "Pressione ESC para voltar ao sistema",
            430,
            725,
            14,
            DARKGRAY
        );

        EndDrawing();
    }

    //Fecha a janela grafica e libera os recursos utilizados pela Raylib
    CloseWindow();
}