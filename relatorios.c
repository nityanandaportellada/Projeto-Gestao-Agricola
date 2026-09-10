//Modulo responsavel pela geracao de relatorios, consultas por periodo e leitura e escrita dos dados do sistema em arquivos CSV e TXT
#include <stdio.h>
#include <sqlite3.h>
#include "relatorios.h"

//Função para gerars relatórios climaticos

void gerarRelatorioClima(sqlite3 *db){
    sqlite3_stmt *stmt;

    //Consulta os registros climaticos juntamente com o nome do talhao e reorganiza a data para realizar a ordenacao cronologica
    const char *sql =
        "SELECT t.nome, c.data, c.hora, "
        "c.temperatura, c.umidade "
        "FROM clima c "
        "INNER JOIN talhoes t "
        "ON c.codigo_talhao = t.codigo "
        "ORDER BY "
        "substr(c.data, 7, 4) || '-' || "
        "substr(c.data, 4, 2) || '-' || "
        "substr(c.data, 1, 2), "
        "c.hora;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao gerar relatorio de clima, tente novamente.\n");
        return;
    }

    printf("\n====================================\n");
    printf("          RELATORIO DE CLIMA\n");
    printf("====================================\n");

    //Percorre todos os registros climaticos encontrados pela consulta
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        printf("\n-----------------------------\n");
        printf("Talhao: %s\n", sqlite3_column_text(stmt, 0));
        printf("Data: %s\n", sqlite3_column_text(stmt, 1));
        printf("Hora: %s\n", sqlite3_column_text(stmt, 2));
        printf("Temperatura: %.1f C\n", sqlite3_column_double(stmt, 3));
        printf("Umidade: %.1f%%\n", sqlite3_column_double(stmt, 4));
    }
    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}


//Geram relatórios que relacionam clima e pragas

void gerarRelatorioPragasClima(sqlite3 *db){
    sqlite3_stmt *stmt;

    //Relaciona ocorrencias, pragas, talhoes e os dados climaticos registrados na mesma data da ocorrencia
    const char *sql =
        "SELECT t.nome, "
        "p.nome, "
        "o.nivel_infestacao, "
        "o.area_afetada, "
        "o.data, "
        "c.temperatura, "
        "c.umidade, "
        "c.data "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "INNER JOIN talhoes t "
        "ON o.codigo_talhao = t.codigo "
        "LEFT JOIN clima c "
        "ON c.id = ("
        "SELECT MAX(c2.id) "
        "FROM clima c2 "
        "WHERE c2.codigo_talhao = o.codigo_talhao "
        "AND c2.data = o.data"
        ") "
        "ORDER BY "
        "substr(o.data, 7, 4) || '-' || "
        "substr(o.data, 4, 2) || '-' || "
        "substr(o.data, 1, 2);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao gerar relatorio de pragas e clima.\n");
        return;
    }

    printf("\n====================================\n");
    printf("       RELATORIO DE PRAGAS E CLIMA\n");
    printf("====================================\n");

    //Percorre todas as ocorrencias encontradas e apresenta os dados das pragas e do clima relacionado
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        printf("\n-----------------------------\n");
        printf("Talhao: %s\n", sqlite3_column_text(stmt, 0));
        printf("Praga: %s\n", sqlite3_column_text(stmt, 1));
        printf("Nivel de infestacao: %d\n", sqlite3_column_int(stmt, 2));
        printf("Area afetada: %.2f hectares\n", sqlite3_column_double(stmt, 3));
        printf("Data da ocorrencia: %s\n", sqlite3_column_text(stmt, 4));

        //Verifica se existe registro climatico associado a data da ocorrencia antes de apresentar esses dados
        if (sqlite3_column_text(stmt, 5) != NULL) {

            printf("Temperatura: %.1f C\n",
                   sqlite3_column_double(stmt, 5));

            printf("Umidade: %.1f%%\n",
                   sqlite3_column_double(stmt, 6));

            printf("Data do clima: %s\n",
                   sqlite3_column_text(stmt, 7));
        } 
        
        else {
            printf("Nao existem dados climaticos para esta data.\n");
        }
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}


//Faz um resumo por período de tempo

void resumoPorPeriodo(sqlite3 *db){
    char dataInicio[20];
    char dataFim[20];

    sqlite3_stmt *stmt;

    //Calcula quantidade, medias e temperaturas maxima e minima somente para os registros existentes dentro do periodo informado
    const char *sql =
        "SELECT COUNT(*), "
        "AVG(temperatura), "
        "AVG(umidade), "
        "MAX(temperatura), "
        "MIN(temperatura) "
        "FROM clima "
        "WHERE "
        "substr(data, 7, 4) || '-' || "
        "substr(data, 4, 2) || '-' || "
        "substr(data, 1, 2) "
        ">= "
        "substr(?, 7, 4) || '-' || "
        "substr(?, 4, 2) || '-' || "
        "substr(?, 1, 2) "
        "AND "
        "substr(data, 7, 4) || '-' || "
        "substr(data, 4, 2) || '-' || "
        "substr(data, 1, 2) "
        "<= "
        "substr(?, 7, 4) || '-' || "
        "substr(?, 4, 2) || '-' || "
        "substr(?, 1, 2);";

    printf("\n====================================\n");
    printf("          RESUMO POR PERIODO\n");
    printf("====================================\n");

    printf("Digite a data inicial (DD/MM/AAAA): ");
    scanf("%19s", dataInicio);

    printf("Digite a data final (DD/MM/AAAA): ");
    scanf("%19s", dataFim);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar periodo.\n");
        return;
    }

    //Associa a data inicial aos tres parametros utilizados para converter dia, mes e ano na consulta SQL
    sqlite3_bind_text(stmt, 1, dataInicio, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, dataInicio, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, dataInicio, -1, SQLITE_TRANSIENT);

    //Associa a data final aos tres parametros utilizados para converter dia, mes e ano na consulta SQL
    sqlite3_bind_text(stmt, 4, dataFim, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, dataFim, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, dataFim, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {

        int quantidade = sqlite3_column_int(stmt, 0);

        printf("\nQuantidade de registros: %d\n",
               quantidade);

        if (quantidade > 0) {
            printf("Temperatura media: %.1f C\n", sqlite3_column_double(stmt, 1));
            printf("Umidade media: %.1f%%\n", sqlite3_column_double(stmt, 2));
            printf("Maior temperatura: %.1f C\n", sqlite3_column_double(stmt, 3));
            printf("Menor temperatura: %.1f C\n", sqlite3_column_double(stmt, 4));
        }
        else {
            printf("Nenhum registro encontrado no periodo.\n");
        }
    }

    sqlite3_finalize(stmt);
}

//Exporta todos os dados do sistema para arquivos CSV
void exportarTalhoesCSV(sqlite3 *db)
{
    FILE *arquivo;
    sqlite3_stmt *stmt;

    //Exportacao dos talhoes
    //APS 2 - Abre o arquivo talhoes.csv no modo de escrita para armazenar os dados dos talhoes
    arquivo = fopen("talhoes.csv", "w");

    if (arquivo == NULL) {
        printf("\nErro ao criar arquivo talhoes.csv.\n");
        return;
    }

    //APS 2 - Utiliza fprintf para escrever o cabecalho do arquivo CSV
    fprintf(arquivo, "Codigo;Nome;Area;Plantacao;Localizacao\n");

    const char *sqlTalhoes =
        "SELECT codigo, nome, area, plantacao, localizacao "
        "FROM talhoes "
        "ORDER BY codigo;";

    if (sqlite3_prepare_v2(db, sqlTalhoes, -1, &stmt, NULL) != SQLITE_OK) {
        printf("\nErro ao consultar talhoes.\n");
        fclose(arquivo);
        return;
    }

    //APS 2 - Percorre os dados recuperados do banco e grava cada registro no arquivo utilizando fprintf
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        fprintf(arquivo, "%d;%s;%.2f;%s;%s\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_text(stmt, 3),
            sqlite3_column_text(stmt, 4));
    }

    sqlite3_finalize(stmt);
    fclose(arquivo);

    //Exportacao dos dados climaticos
    //APS 2 - Abre o arquivo clima.csv no modo de escrita para armazenar os registros climaticos
    arquivo = fopen("clima.csv", "w");

    if (arquivo == NULL) {
        printf("\nErro ao criar arquivo clima.csv.\n");
        return;
    }

    fprintf(
        arquivo,
        "ID;Codigo Talhao;Talhao;Temperatura;Umidade;Data;Hora\n"
    );

    const char *sqlClima =
        "SELECT c.id, c.codigo_talhao, t.nome, "
        "c.temperatura, c.umidade, c.data, c.hora "
        "FROM clima c "
        "INNER JOIN talhoes t "
        "ON c.codigo_talhao = t.codigo "
        "ORDER BY c.id;";

    if (sqlite3_prepare_v2(db, sqlClima, -1, &stmt, NULL) != SQLITE_OK) {
        printf("\nErro ao consultar dados climaticos.\n");
        fclose(arquivo);
        return;
    }


    //APS 2 - Grava cada registro climatico recuperado do banco no arquivo clima.csv
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        fprintf(
            arquivo,
            "%d;%d;%s;%.1f;%.1f;%s;%s\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_double(stmt, 3),
            sqlite3_column_double(stmt, 4),
            sqlite3_column_text(stmt, 5),
            sqlite3_column_text(stmt, 6)
        );
    }

    sqlite3_finalize(stmt);
    fclose(arquivo);

    //Exportacao das pragas cadastradas
    //APS 2 - Abre o arquivo pragas.csv no modo de escrita para armazenar as pragas cadastradas
    arquivo = fopen("pragas.csv", "w");

    if (arquivo == NULL) {
        printf("\nErro ao criar arquivo pragas.csv.\n");
        return;
    }

    fprintf(
        arquivo,
        "Codigo;Nome;Descricao;Nivel Risco\n"
    );

    const char *sqlPragas =
        "SELECT codigo, nome, descricao, nivel_risco "
        "FROM pragas "
        "ORDER BY codigo;";

    if (sqlite3_prepare_v2(db, sqlPragas, -1, &stmt, NULL) != SQLITE_OK) {
        printf("\nErro ao consultar pragas.\n");
        fclose(arquivo);
        return;
    }

    //APS 2 - Grava cada praga recuperada do banco no arquivo pragas.csv
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        fprintf(
            arquivo,
            "%d;%s;%s;%d\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_int(stmt, 3)
        );
    }

    sqlite3_finalize(stmt);
    fclose(arquivo);

    //Exportacao das ocorrencias de pragas
    //APS 2 - Abre o arquivo ocorrencias_pragas.csv no modo de escrita para armazenar as ocorrencias cadastradas
    arquivo = fopen("ocorrencias_pragas.csv", "w");

    if (arquivo == NULL) {
        printf("\nErro ao criar arquivo ocorrencias_pragas.csv.\n");
        return;
    }

    fprintf(
        arquivo,
        "Codigo;Codigo Praga;Praga;Codigo Talhao;Talhao;Nivel Infestacao;Area Afetada;Data\n"
    );

    //Relaciona ocorrencias, pragas e talhoes para exportar os dados completos de cada ocorrencia
    const char *sqlOcorrencias =
        "SELECT o.codigo, o.codigo_praga, p.nome, "
        "o.codigo_talhao, t.nome, o.nivel_infestacao, "
        "o.area_afetada, o.data "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "INNER JOIN talhoes t "
        "ON o.codigo_talhao = t.codigo "
        "ORDER BY o.codigo;";

    if (sqlite3_prepare_v2(db, sqlOcorrencias, -1, &stmt, NULL) != SQLITE_OK) {
        printf("\nErro ao consultar ocorrencias de pragas.\n");
        fclose(arquivo);
        return;
    }

    //APS 2 - Grava cada ocorrencia de praga recuperada do banco no arquivo ocorrencias_pragas.csv
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        fprintf(
            arquivo,
            "%d;%d;%s;%d;%s;%d;%.2f;%s\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_text(stmt, 4),
            sqlite3_column_int(stmt, 5),
            sqlite3_column_double(stmt, 6),
            sqlite3_column_text(stmt, 7)
        );
    }

    sqlite3_finalize(stmt);
    fclose(arquivo);

    printf("\nDados exportados com sucesso!\n");
    printf("Arquivos criados:\n");
    printf("talhoes.csv\n");
    printf("clima.csv\n");
    printf("pragas.csv\n");
    printf("ocorrencias_pragas.csv\n");
}

//Le as informacoes do arquivo CSV
void lerTalhoesCSV()
{
    FILE *arquivo;
    char linha[500];

    //APS 2 - Abre o arquivo talhoes.csv no modo de leitura para recuperar as informacoes armazenadas
    arquivo = fopen("talhoes.csv", "r");

    if (arquivo == NULL) {
        printf("\nArquivo talhoes.csv nao encontrado.\n");
    }
    else {
        printf("\n====================================\n");
        printf("              TALHOES\n");
        printf("====================================\n");

        //APS 2 - Utiliza fgets para ler cada linha do arquivo ate chegar ao final
        while (fgets(linha, sizeof(linha), arquivo) != NULL) {
            printf("%s", linha);
        }

        fclose(arquivo);
    }

    //APS 2 - Abre o arquivo clima.csv no modo de leitura
    arquivo = fopen("clima.csv", "r");

    if (arquivo == NULL) {
        printf("\nArquivo clima.csv nao encontrado.\n");
    }
    else {
        printf("\n====================================\n");
        printf("               CLIMA\n");
        printf("====================================\n");

        //APS 2 - Le sequencialmente todas as linhas armazenadas no arquivo clima.csv
        while (fgets(linha, sizeof(linha), arquivo) != NULL) {
            printf("%s", linha);
        }

        fclose(arquivo);
    }

    //APS 2 - Abre o arquivo pragas.csv no modo de leitura
    arquivo = fopen("pragas.csv", "r");

    if (arquivo == NULL) {
        printf("\nArquivo pragas.csv nao encontrado.\n");
    }
    else {
        printf("\n====================================\n");
        printf("              PRAGAS\n");
        printf("====================================\n");

        //APS 2 - Le sequencialmente todas as linhas armazenadas no arquivo pragas.csv
        while (fgets(linha, sizeof(linha), arquivo) != NULL) {
            printf("%s", linha);
        }

        fclose(arquivo);
    }

    //APS 2 - Abre o arquivo ocorrencias_pragas.csv no modo de leitura
    arquivo = fopen("ocorrencias_pragas.csv", "r");

    if (arquivo == NULL) {
        printf("\nArquivo ocorrencias_pragas.csv nao encontrado.\n");
    }
    else {
        printf("\n====================================\n");
        printf("       OCORRENCIAS DE PRAGAS\n");
        printf("====================================\n");

        //APS 2 - Le sequencialmente todas as linhas armazenadas no arquivo ocorrencias_pragas.csv
        while (fgets(linha, sizeof(linha), arquivo) != NULL) {
            printf("%s", linha);
        }

        fclose(arquivo);
    }
}

//Exporta o relatorio geral da fazenda para arquivo TXT
void exportarRelatorioTXT(sqlite3 *db)
{
    FILE *arquivo;
    sqlite3_stmt *stmt;

    //APS 2 - Abre o arquivo relatorio_fazenda.txt no modo de escrita para armazenar o relatorio geral
    arquivo = fopen("relatorio_fazenda.txt", "w");

    if (arquivo == NULL) {
        printf("\nErro ao criar o arquivo de relatorio.\n");
        return;
    }

    //APS 2 - Utiliza fprintf para escrever o conteudo do relatorio diretamente no arquivo TXT
    fprintf(arquivo, "============================================\n");
    fprintf(arquivo, "          RELATORIO GERAL DA FAZENDA\n");
    fprintf(arquivo, "============================================\n\n");

    fprintf(arquivo, "============== TALHOES ==============\n\n");

    const char *sqlTalhoes =
        "SELECT codigo, nome, area, plantacao, localizacao "
        "FROM talhoes "
        "ORDER BY codigo;";

    if (sqlite3_prepare_v2(db, sqlTalhoes, -1, &stmt, NULL) == SQLITE_OK) {

        //APS 2 - Grava no arquivo TXT os dados de cada talhao recuperado do banco
        while (sqlite3_step(stmt) == SQLITE_ROW) {

            fprintf(arquivo, "Codigo: %d\n",
                sqlite3_column_int(stmt, 0));

            fprintf(arquivo, "Nome: %s\n",
                sqlite3_column_text(stmt, 1));

            fprintf(arquivo, "Area: %.2f ha\n",
                sqlite3_column_double(stmt, 2));

            fprintf(arquivo, "Plantacao: %s\n",
                sqlite3_column_text(stmt, 3));

            fprintf(arquivo, "Localizacao: %s\n",
                sqlite3_column_text(stmt, 4));

            fprintf(arquivo, "--------------------------------------------\n");
        }

        sqlite3_finalize(stmt);
    }

    fprintf(arquivo, "\n============== CLIMA ==============\n\n");

    const char *sqlClima =
        "SELECT t.nome, c.data, c.hora, "
        "c.temperatura, c.umidade "
        "FROM clima c "
        "INNER JOIN talhoes t "
        "ON c.codigo_talhao = t.codigo "
        "ORDER BY "
        "substr(c.data, 7, 4) || '-' || "
        "substr(c.data, 4, 2) || '-' || "
        "substr(c.data, 1, 2), "
        "c.hora;";

    if (sqlite3_prepare_v2(db, sqlClima, -1, &stmt, NULL) == SQLITE_OK) {

        //APS 2 - Grava no arquivo TXT os registros climaticos recuperados do banco
        while (sqlite3_step(stmt) == SQLITE_ROW) {

            fprintf(arquivo, "Talhao: %s\n",
                sqlite3_column_text(stmt, 0));

            fprintf(arquivo, "Data: %s\n",
                sqlite3_column_text(stmt, 1));

            fprintf(arquivo, "Hora: %s\n",
                sqlite3_column_text(stmt, 2));

            fprintf(arquivo, "Temperatura: %.1f C\n",
                sqlite3_column_double(stmt, 3));

            fprintf(arquivo, "Umidade: %.1f%%\n",
                sqlite3_column_double(stmt, 4));

            fprintf(arquivo, "--------------------------------------------\n");
        }

        sqlite3_finalize(stmt);
    }

    fprintf(arquivo, "\n============== PRAGAS ==============\n\n");

    const char *sqlPragas =
        "SELECT t.nome, "
        "p.nome, "
        "o.nivel_infestacao, "
        "o.area_afetada, "
        "o.data "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "INNER JOIN talhoes t "
        "ON o.codigo_talhao = t.codigo "
        "ORDER BY "
        "substr(o.data, 7, 4) || '-' || "
        "substr(o.data, 4, 2) || '-' || "
        "substr(o.data, 1, 2);";

    if (sqlite3_prepare_v2(db, sqlPragas, -1, &stmt, NULL) == SQLITE_OK) {

        //APS 2 - Grava no arquivo TXT as ocorrencias de pragas recuperadas do banco
        while (sqlite3_step(stmt) == SQLITE_ROW) {

            fprintf(arquivo, "Talhao: %s\n",
                sqlite3_column_text(stmt, 0));

            fprintf(arquivo, "Praga: %s\n",
                sqlite3_column_text(stmt, 1));

            fprintf(arquivo, "Nivel de infestacao: %d\n",
                sqlite3_column_int(stmt, 2));

            fprintf(arquivo, "Area afetada: %.2f hectares\n",
                sqlite3_column_double(stmt, 3));

            fprintf(arquivo, "Data: %s\n",
                sqlite3_column_text(stmt, 4));

            fprintf(arquivo, "--------------------------------------------\n");
        }

        sqlite3_finalize(stmt);
    }

    //APS 2 - Fecha o arquivo TXT apos finalizar toda a escrita do relatorio
    fclose(arquivo);

    printf("\nRelatorio exportado com sucesso!\n");
    printf("Arquivo criado: relatorio_fazenda.txt\n");
}