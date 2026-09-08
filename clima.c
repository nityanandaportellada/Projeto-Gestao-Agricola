#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "clima.h"
#include "talhoes.h"

//funcoes para calcular os riscos de temperatura
int calcularRiscoTemperatura(float temperatura)
{
    if (temperatura >= 20 && temperatura <= 30) {
        return 1;
    } else if (temperatura > 30 && temperatura <= 35) {
        return 2;
    } else if (temperatura >= 15 && temperatura < 20) {
        return 2;
    } else if (temperatura > 35 && temperatura <= 40) {
        return 3;
    } else if (temperatura >= 5 && temperatura < 15) {
        return 3;
    } else {
        return 4;
    }
}

//funcoes para calcular os riscos de temperatura
int calcularRiscoUmidade(float umidade)
{
    if (umidade >= 60 && umidade <= 65) {
        return 1;
    } else if (umidade >= 50 && umidade < 60) {
        return 2;
    } else if (umidade >= 40 && umidade < 50) {
        return 3;
    } else if (umidade > 65 && umidade <= 70) {
        return 3;
    } else {
        return 4;
    }
}

//Função que calcula os riscos conjuntos dos dois fatores
void calcularRiscosClimaticos(float temperatura, float umidade, int *riscoTemperatura, int *riscoUmidade) {
    *riscoTemperatura = calcularRiscoTemperatura(temperatura);
    *riscoUmidade = calcularRiscoUmidade(umidade);
}

//função que vai registrar os climas encontrados
void registrarClima(sqlite3 *db)
{
    int codigoTalhao;
    int riscoTemperatura;
    int riscoUmidade;

    float temperatura;
    float umidade;

    char data[12];
    char hora[7];

    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("       REGISTRO DE DADOS CLIMATICOS\n");
    printf("====================================\n");

    const char *sqlQuantidade =
        "SELECT COUNT(*) FROM talhoes;";

    if (sqlite3_prepare_v2(db, sqlQuantidade, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar talhoes.\n");
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {

        int quantidade = sqlite3_column_int(stmt, 0);

        if (quantidade == 0) {
            printf("\nNenhum talhao cadastrado.\n");
            printf("Cadastre um talhao antes de registrar o clima.\n");

            sqlite3_finalize(stmt);

            return;
        }
    }

    sqlite3_finalize(stmt);

    printf("Digite o codigo do talhao: ");

    if (scanf("%d", &codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');

        return;
    }

    if (buscarTalhao(db, codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        printf("Cadastre o talhao antes de registrar o clima.\n");

        return;
    }

    printf("Digite a temperatura (C): ");

    if (scanf("%f", &temperatura) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    if (temperatura < -20 || temperatura > 50) {
        printf("\nTemperatura invalida!\n");
        printf("Digite um valor entre -20 e 50 graus Celsius.\n");

        return;
    }

    printf("Digite a umidade relativa (Em Porcentagem): ");

    if (scanf("%f", &umidade) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    if (umidade < 0 || umidade > 100) {
        printf("\nUmidade invalida!\n");
        printf("Digite um valor entre 0 e 100%%.\n");

        return;
    }

    calcularRiscosClimaticos(temperatura, umidade, &riscoTemperatura, &riscoUmidade);

    printf("\nRisco da temperatura: %d\n", riscoTemperatura);
    printf("Risco da umidade: %d\n", riscoUmidade);

    getchar();

    printf("Digite a data (DD/MM/AAAA): ");
    fgets(data, 12, stdin);

    data[strcspn(data, "\n")] = '\0';

    if (!validarData(data)) {
        printf("\nData invalida.\n");
        printf("Utilize o formato DD/MM/AAAA.\n");

        return;
    }

    printf("Digite a hora (HH:MM): ");
    fgets(hora, 7, stdin);

    hora[strcspn(hora, "\n")] = '\0';

    if (!validarHora(hora)) {
        printf("\nHora invalida.\n");
        printf("Utilize o formato HH:MM.\n");

        return;
    }

    const char *sql =
        "INSERT INTO clima "
        "(codigo_talhao, temperatura, umidade, data, hora) "
        "VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao inserir registro climatico.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigoTalhao);
    sqlite3_bind_double(stmt, 2, temperatura);
    sqlite3_bind_double(stmt, 3, umidade);
    sqlite3_bind_text(stmt, 4, data, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, hora, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nRegistro climatico salvo com sucesso!\n");
    }
    else {
        printf("\nErro ao salvar registro climatico.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//Função para listar o historico de clima cadastrado
void listarHistoricoClima(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT c.id, "
        "c.codigo_talhao, "
        "t.nome, "
        "c.data, "
        "c.hora, "
        "c.temperatura, "
        "c.umidade "
        "FROM clima c "
        "INNER JOIN talhoes t "
        "ON c.codigo_talhao = t.codigo "
        "ORDER BY "
        "substr(c.data, 7, 4) || '-' || "
        "substr(c.data, 4, 2) || '-' || "
        "substr(c.data, 1, 2), "
        "c.hora;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {

        printf("Erro ao consultar historico climatico.\n");
        return;
    }

    printf("\n====================================\n");
    printf("          HISTORICO CLIMATICO\n");
    printf("====================================\n");

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        int id = sqlite3_column_int(stmt, 0);
        int codigoTalhao = sqlite3_column_int(stmt, 1);
        const char *nomeTalhao = (const char *)sqlite3_column_text(stmt, 2);
        const char *data = (const char *)sqlite3_column_text(stmt, 3);
        const char *hora = (const char *)sqlite3_column_text(stmt, 4);
        float temperatura = sqlite3_column_double(stmt, 5);
        float umidade = sqlite3_column_double(stmt, 6);

        int riscoTemperatura = calcularRiscoTemperatura(temperatura);
        int riscoUmidade = calcularRiscoUmidade(umidade);

        printf("\n-----------------------------\n");
        printf("ID: %d\n", id);
        printf("Talhao: %d\n", codigoTalhao);
        printf("Nome: %s\n", nomeTalhao);
        printf("Data: %s\n", data);
        printf("Hora: %s\n", hora);
        printf("Temperatura: %.1f C\n", temperatura);
        printf("Risco temperatura: %d\n", riscoTemperatura);
        printf("Umidade: %.1f%%\n", umidade);
        printf("Risco umidade: %d\n", riscoUmidade);
    }

    if (!encontrou) {
        printf("\nNenhum registro climatico encontrado.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}

//Função que visualize a serie de clima por talhao

void visualizarSerieTalhao(sqlite3 *db)
{
    int codigo;
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT data, hora, temperatura, umidade "
        "FROM clima "
        "WHERE codigo_talhao = ? "
        "ORDER BY "
        "substr(data, 7, 4) || '-' || "
        "substr(data, 4, 2) || '-' || "
        "substr(data, 1, 2), "
        "hora;";

    printf("\n==============================================\n");
    printf("       SERIE CLIMATICA HISTORICA POR TALHAO\n");
    printf("==============================================\n");

    printf("Digite o codigo do talhao: ");

    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Verifica se o talhao existe.
    if (buscarTalhao(db, codigo) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar serie historica.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    int encontrou = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;
        printf("\n-----------------------------\n");
        printf("Data: %s   Hora: %s\n", sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1));
        printf("Temperatura: %.1f C   Umidade: %.1f%%\n", sqlite3_column_double(stmt, 2), sqlite3_column_double(stmt, 3));
    }


    if (!encontrou) {
        printf("\nNenhum registro encontrado para esse talhao.\n");
    } 
    
    else {
        printf("-----------------------------\n");
    }
    sqlite3_finalize(stmt);
}

//funcao para editar clima ja cadastrado
void editarClima(sqlite3 *db)
{
    int id;
    int codigoTalhao;
    int riscoTemperatura;
    int riscoUmidade;

    float temperatura;
    float umidade;

    char data[12];
    char hora[7];

    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          EDITAR CLIMA\n");
    printf("====================================\n");

    printf("Digite o ID do registro climatico: ");

    if (scanf("%d", &id) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    const char *sqlBusca =
        "SELECT id FROM clima WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sqlBusca, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar registro climatico.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("\nRegistro climatico nao encontrado.\n");

        sqlite3_finalize(stmt);

        return;
    }

    sqlite3_finalize(stmt);

    printf("Digite o codigo do talhao: ");

    if (scanf("%d", &codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    if (buscarTalhao(db, codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    printf("Digite a temperatura em Graus (C): ");

    if (scanf("%f", &temperatura) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    if (temperatura < -20 || temperatura > 50) {
        printf("\nTemperatura invalida!\n");
        printf("Digite um valor entre -20 e 50 graus Celsius.\n");

        return;
    }

    printf("Digite a umidade relativa (Em Porcentagem): ");

    if (scanf("%f", &umidade) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');

        return;
    }

    if (umidade < 0 || umidade > 100) {
        printf("\nUmidade invalida!\n");
        printf("Digite um valor entre 0 e 100%%.\n");

        return;
    }

    calcularRiscosClimaticos(temperatura, umidade, &riscoTemperatura, &riscoUmidade);

    printf("\nRisco da temperatura: %d\n", riscoTemperatura);
    printf("Risco da umidade: %d\n", riscoUmidade);

    getchar();

    printf("Digite a data (DD/MM/AAAA): ");
    fgets(data, 12, stdin);

    data[strcspn(data, "\n")] = '\0';

    if (!validarData(data)) {
        printf("\nData invalida.\n");
        printf("Utilize o formato DD/MM/AAAA.\n");

        return;
    }

    printf("Digite a hora (HH:MM): ");
    fgets(hora, 7, stdin);

    hora[strcspn(hora, "\n")] = '\0';

    if (!validarHora(hora)) {
        printf("\nHora invalida.\n");
        printf("Utilize o formato HH:MM.\n");

        return;
    }

    const char *sql =
        "UPDATE clima "
        "SET codigo_talhao = ?, "
        "temperatura = ?, "
        "umidade = ?, "
        "data = ?, "
        "hora = ? "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar edicao do clima.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigoTalhao);
    sqlite3_bind_double(stmt, 2, temperatura);
    sqlite3_bind_double(stmt, 3, umidade);
    sqlite3_bind_text(stmt, 4, data, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, hora, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nRegistro climatico editado com sucesso!\n");
    }
    else {
        printf("\nErro ao editar registro climatico.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//funcao para excluir clima ja cadastrado
void excluirClima(sqlite3 *db)
{
    int id;
    char confirmacao;
    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          EXCLUIR CLIMA\n");
    printf("====================================\n");

    printf("Digite o ID do registro climatico: ");

    if (scanf("%d", &id) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    const char *sqlBusca =
        "SELECT id FROM clima WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sqlBusca, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar registro climatico.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("\nRegistro climatico nao encontrado.\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    printf("Deseja realmente excluir este registro? (S/N): ");
    scanf(" %c", &confirmacao);

    if (confirmacao != 'S' && confirmacao != 's') {
        printf("Exclusao cancelada.\n");
        return;
    }

    const char *sql =
        "DELETE FROM clima "
        "WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar exclusao do clima.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nRegistro climatico excluido com sucesso!\n");
    } else {
        printf("\nErro ao excluir registro climatico.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

int validarData(char data[])
{
    int dia;
    int mes;
    int ano;
    int diasNoMes;

    if (strlen(data) != 10) {
        return 0;
    }

    if (data[2] != '/' || data[5] != '/') {
        return 0;
    }

    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) {
        return 0;
    }

    if (mes < 1 || mes > 12) {
        return 0;
    }

    if (ano < 2000 || ano > 2100) {
        return 0;
    }

    if (mes == 2) {

        if ((ano % 400 == 0) || (ano % 4 == 0 && ano % 100 != 0)) {
            diasNoMes = 29;
        }
        else {
            diasNoMes = 28;
        }
    }
    else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        diasNoMes = 30;
    }
    else {
        diasNoMes = 31;
    }

    if (dia < 1 || dia > diasNoMes) {
        return 0;
    }

    return 1;
}

int validarHora(char hora[])
{
    int horas;
    int minutos;

    if (strlen(hora) != 5) {
        return 0;
    }

    if (hora[2] != ':') {
        return 0;
    }

    if (sscanf(hora, "%d:%d", &horas, &minutos) != 2) {
        return 0;
    }

    if (horas < 0 || horas > 23) {
        return 0;
    }

    if (minutos < 0 || minutos > 59) {
        return 0;
    }

    return 1;
}