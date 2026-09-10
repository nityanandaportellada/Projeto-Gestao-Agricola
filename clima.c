//Modulo responsavel pelo registro, consulta, edicao, exclusao, validacao e calculo dos riscos relacionados aos dados climaticos dos talhoes
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
//APS 1 - Utiliza ponteiros para alterar simultaneamente as variaveis que recebem os riscos de temperatura e umidade
void calcularRiscosClimaticos(float temperatura, float umidade, int *riscoTemperatura, int *riscoUmidade) {
    //Armazena diretamente nas variaveis externas os dois resultados calculados por meio dos ponteiros recebidos
    *riscoTemperatura = calcularRiscoTemperatura(temperatura);
    *riscoUmidade = calcularRiscoUmidade(umidade);
}

//Registra um novo conjunto de dados climaticos associado a um talhao existente
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

    //Consulta quantos talhoes existem para impedir o registro de clima quando nenhum talhao estiver cadastrado
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

    //Valida se a entrada do codigo do talhao foi realizada utilizando um numero inteiro
    if (scanf("%d", &codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');

        return;
    }

    //Verifica se o talhao informado realmente existe antes de permitir o registro climatico
    if (buscarTalhao(db, codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        printf("Cadastre o talhao antes de registrar o clima.\n");

        return;
    }

    printf("Digite a temperatura (C): ");

    //Valida se a temperatura informada possui um valor numerico
    if (scanf("%f", &temperatura) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Limita a temperatura aos valores considerados validos pelo sistema
    if (temperatura < -20 || temperatura > 50) {
        printf("\nTemperatura invalida!\n");
        printf("Digite um valor entre -20 e 50 graus Celsius.\n");

        return;
    }

    printf("Digite a umidade relativa (Em Porcentagem): ");

    //Valida se a umidade informada possui um valor numerico
    if (scanf("%f", &umidade) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Limita a umidade relativa aos valores possiveis entre 0 e 100
    if (umidade < 0 || umidade > 100) {
        printf("\nUmidade invalida!\n");
        printf("Digite um valor entre 0 e 100%%.\n");

        return;
    }

    //APS 1 - Envia os enderecos das duas variaveis para que seus valores sejam alterados pela funcao utilizando ponteiros
    calcularRiscosClimaticos(temperatura, umidade, &riscoTemperatura, &riscoUmidade);

    printf("\nRisco da temperatura: %d\n", riscoTemperatura);
    printf("Risco da umidade: %d\n", riscoUmidade);

    getchar();

    printf("Digite a data (DD/MM/AAAA): ");
    fgets(data, 12, stdin);

    //Remove o caractere de quebra de linha armazenado pelo fgets
    data[strcspn(data, "\n")] = '\0';

    //Valida se a data possui o formato e os valores aceitos pelo sistema
    if (!validarData(data)) {
        printf("\nData invalida.\n");
        printf("Utilize o formato DD/MM/AAAA.\n");

        return;
    }

    printf("Digite a hora (HH:MM): ");
    fgets(hora, 7, stdin);

    //Remove o caractere de quebra de linha armazenado pelo fgets
    hora[strcspn(hora, "\n")] = '\0';

    //Valida se a hora possui o formato e os valores aceitos pelo sistema
    if (!validarHora(hora)) {
        printf("\nHora invalida.\n");
        printf("Utilize o formato HH:MM.\n");

        return;
    }

    //Prepara o comando SQL que insere o novo registro climatico no banco de dados
    const char *sql =
        "INSERT INTO clima "
        "(codigo_talhao, temperatura, umidade, data, hora) "
        "VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao inserir registro climatico.\n");
        return;
    }

    //Associa os valores informados aos parametros representados pelos sinais de interrogacao do comando SQL
    sqlite3_bind_int(stmt, 1, codigoTalhao);
    sqlite3_bind_double(stmt, 2, temperatura);
    sqlite3_bind_double(stmt, 3, umidade);
    sqlite3_bind_text(stmt, 4, data, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, hora, -1, SQLITE_TRANSIENT);

    //Executa o comando de insercao e verifica se o registro foi salvo corretamente
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nRegistro climatico salvo com sucesso!\n");
    }
    else {
        printf("\nErro ao salvar registro climatico.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//Lista todos os registros climaticos cadastrados juntamente com as informacoes dos respectivos talhoes
void listarHistoricoClima(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    //Relaciona os registros climaticos com os talhoes e converte a data armazenada em DD/MM/AAAA para realizar a ordenacao cronologica
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

    //Controla se pelo menos um registro climatico foi encontrado na consulta
    int encontrou = 0;

    //Percorre todos os registros retornados pela consulta
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        //Recupera os valores de cada coluna do registro climatico atual
        int id = sqlite3_column_int(stmt, 0);
        int codigoTalhao = sqlite3_column_int(stmt, 1);
        const char *nomeTalhao = (const char *)sqlite3_column_text(stmt, 2);
        const char *data = (const char *)sqlite3_column_text(stmt, 3);
        const char *hora = (const char *)sqlite3_column_text(stmt, 4);
        float temperatura = sqlite3_column_double(stmt, 5);
        float umidade = sqlite3_column_double(stmt, 6);

        //Calcula os riscos correspondentes a temperatura e umidade do registro apresentado
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

    //Informa ao usuario quando a consulta nao encontrar nenhum registro climatico
    if (!encontrou) {
        printf("\nNenhum registro climatico encontrado.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}

//Exibe em ordem cronologica todos os registros climaticos pertencentes ao talhao informado pelo usuario
void visualizarSerieTalhao(sqlite3 *db)
{
    int codigo;
    sqlite3_stmt *stmt;

    //Seleciona os registros do talhao informado e reorganiza a data DD/MM/AAAA para permitir a ordenacao cronologica correta
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

    //Valida se o codigo informado pelo usuario e numerico
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

    //Associa o codigo do talhao ao parametro utilizado na consulta SQL
    sqlite3_bind_int(stmt, 1, codigo);

    //Controla se algum registro foi encontrado para o talhao informado
    int encontrou = 0;

    //Percorre e apresenta todos os registros climaticos encontrados
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

//Permite localizar um registro climatico pelo ID e substituir os dados armazenados
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

    //Valida se o ID informado pelo usuario e numerico
    if (scanf("%d", &id) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Consulta o banco para verificar se o ID informado realmente pertence a um registro climatico
    const char *sqlBusca =
        "SELECT id FROM clima WHERE id = ?;";

    if (sqlite3_prepare_v2(db, sqlBusca, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar registro climatico.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    //Interrompe a edicao quando o registro informado nao for encontrado
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

    //Verifica se o novo codigo informado pertence a um talhao existente
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

    //Valida os limites de temperatura aceitos pelo sistema
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

    //Valida os limites de umidade aceitos pelo sistema
    if (umidade < 0 || umidade > 100) {
        printf("\nUmidade invalida!\n");
        printf("Digite um valor entre 0 e 100%%.\n");

        return;
    }

    //APS 1 - Recalcula simultaneamente os riscos climaticos utilizando os enderecos das variaveis enviados por ponteiros
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

    //Prepara o comando UPDATE utilizado para substituir os dados do registro climatico selecionado
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

    //Associa os novos dados e o ID do registro aos parametros utilizados pelo comando UPDATE
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

//Permite localizar pelo ID e excluir um registro climatico armazenado no banco
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

    //Consulta o banco para verificar se existe um registro climatico com o ID informado
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

    //Solicita confirmacao antes de excluir definitivamente o registro
    printf("Deseja realmente excluir este registro? (S/N): ");
    scanf(" %c", &confirmacao);

    if (confirmacao != 'S' && confirmacao != 's') {
        printf("Exclusao cancelada.\n");
        return;
    }

    //Prepara o comando SQL utilizado para excluir o registro correspondente ao ID informado
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

//Valida se a data informada possui o formato DD/MM/AAAA e se os valores representam uma data valida
int validarData(char data[])
{
    int dia;
    int mes;
    int ano;
    int diasNoMes;

    //Verifica se a data possui exatamente os dez caracteres esperados no formato DD/MM/AAAA
    if (strlen(data) != 10) {
        return 0;
    }

    //Confirma se as barras separadoras estao posicionadas corretamente
    if (data[2] != '/' || data[5] != '/') {
        return 0;
    }

    //Converte as partes da string em dia, mes e ano para permitir suas validacoes
    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) {
        return 0;
    }

    if (mes < 1 || mes > 12) {
        return 0;
    }

    if (ano < 2000 || ano > 2100) {
        return 0;
    }

    //Define corretamente a quantidade de dias de fevereiro considerando a regra dos anos bissextos
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

    //Verifica se o dia informado esta dentro da quantidade permitida para o mes
    if (dia < 1 || dia > diasNoMes) {
        return 0;
    }

    return 1;
}

//Valida se a hora informada possui o formato HH:MM e valores validos
int validarHora(char hora[])
{
    int horas;
    int minutos;

    //Verifica se a hora possui exatamente os cinco caracteres esperados no formato HH:MM
    if (strlen(hora) != 5) {
        return 0;
    }

    //Confirma se o caractere de separacao entre horas e minutos esta na posicao correta
    if (hora[2] != ':') {
        return 0;
    }

    //Converte a string em valores numericos de horas e minutos para permitir suas validacoes
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