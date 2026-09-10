//Modulo responsavel pelo cadastro, consulta, edicao e exclusao de pragas e de suas ocorrencias nos talhoes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "pragas.h"
#include "talhoes.h"
#include "clima.h"


//Função para listar as pragas cadastradas
void listarPragas(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    //Seleciona os dados das pragas cadastradas e organiza os resultados pelo codigo
    const char *sql =
        "SELECT codigo, nome, descricao, nivel_risco "
        "FROM pragas "
        "ORDER BY codigo;";

    //verifica se tem pragas cadastradas
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar pragas.\n");
        return;
    }

    printf("\n====================================\n");
    printf("          PRAGAS CADASTRADAS\n");
    printf("====================================\n");

    //Controla se pelo menos uma praga foi encontrada durante a consulta
    int encontrou = 0;

    //Percorre todas as pragas retornadas pela consulta
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        printf("\n-----------------------------\n");
        printf("Codigo: %d\n", sqlite3_column_int(stmt, 0));
        printf("Nome: %s\n", sqlite3_column_text(stmt, 1));
        printf("O que ela faz: %s\n", sqlite3_column_text(stmt, 2));
        printf("Nivel de risco: %d\n", sqlite3_column_int(stmt, 3));
    }


    if (!encontrou) {
        printf("\nNenhuma praga cadastrada.\n");
    }

    printf("-----------------------------\n");
    sqlite3_finalize(stmt);
}

//Função para cadastrar uma praga
void cadastrarPraga(sqlite3 *db)
{
    int codigo;
    int nivelRisco;
    char nome[50];
    char descricao[200];
    char entrada[20];
    int valido;

    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          CADASTRO DE PRAGA\n");
    printf("====================================\n");

    //Cadastra o codigo da praga
    do {
        valido = 1;

        printf("Digite o codigo da praga: ");
        scanf("%19s", entrada);
        //Percorre cada caractere digitado para garantir que o codigo contenha somente numeros
        for (int i = 0; entrada[i] != '\0'; i++) {
            if (entrada[i] < '0' || entrada[i] > '9') {
                valido = 0;
                break;
            }
        }

        if (!valido) {
            printf("\nERRO: Digite somente um numero inteiro!\n");
        }

    } while (!valido);

    //Converte o codigo validado de texto para numero inteiro
    codigo = atoi(entrada);

    //Verifica se o codigo cadastrado ja existe
    if (buscarPraga(db, codigo) != -1) {
        printf("\nERRO: Ja existe uma praga com este codigo!\n");
        return;
    }

    getchar();

    //Cadastra o nome da praga
    printf("Digite o nome da praga: ");
    fgets(nome, 50, stdin);
    nome[strcspn(nome, "\n")] = '\0';

    //Cadastra o que a praga faz
    printf("Digite o que a praga faz: ");
    fgets(descricao, 200, stdin);

    descricao[strcspn(descricao, "\n")] = '\0';

    //Cadastra o nivel de risco da praga
    printf("Digite o nivel de risco (1 a 4): ");

    if (scanf("%d", &nivelRisco) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Verifica se o nivel inserido esta dentro do esperado
    if (nivelRisco < 1 || nivelRisco > 4) {
        printf("\nNivel de risco invalido!\n");
        printf("Digite um valor entre 1 e 4.\n");
        return;
    }

    //Insere os dados da praga no banco
    const char *sql =
        "INSERT INTO pragas "
        "(codigo, nome, descricao, nivel_risco) "
        "VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao cadastrar praga.\n");
        return;
    }

    //Associa os dados da nova praga aos parametros do comando SQL preparado
    sqlite3_bind_int(stmt, 1, codigo);
    sqlite3_bind_text(
        stmt,
        2,
        nome,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        stmt,
        3,
        descricao,
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(stmt, 4, nivelRisco);

    //Executa a inclusao da praga no banco de dados
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nPraga cadastrada com sucesso!\n");

    }

    else {
        printf("\nErro ao cadastrar praga.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//Função que busca uma praga cadastrada
int buscarPraga(sqlite3 *db, int codigo)
{
    sqlite3_stmt *stmt;

    //Consulta a existencia de uma praga utilizando seu codigo como criterio
    const char *sql =
        "SELECT codigo "
        "FROM pragas "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    //Retorna 1 quando o codigo da praga for encontrado
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 1;
    }

    //Retorna -1 quando nenhuma praga com o codigo informado for encontrada
    sqlite3_finalize(stmt);
    return -1;
}

//Função para cadastrar uma ocorrência
void cadastrarOcorrencia(sqlite3 *db)
{
    OcorrenciaPraga ocorrencia;

    int quantidadePragas;
    int quantidadeTalhoes;

    float areaTalhao = 0;

    sqlite3_stmt *stmt;
    sqlite3_stmt *stmtArea;

    printf("\n====================================\n");
    printf("       CADASTRO DE OCORRENCIA\n");
    printf("====================================\n");

    //Consulta a quantidade de pragas existentes antes de permitir o cadastro de uma ocorrencia
    const char *sqlQuantidadePragas =
        "SELECT COUNT(*) FROM pragas;";

    if (sqlite3_prepare_v2(db, sqlQuantidadePragas, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar pragas.\n");
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        quantidadePragas = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    //Impede o cadastro de ocorrencias quando nenhuma praga estiver cadastrada
    if (quantidadePragas == 0) {
        printf("\nNenhuma praga cadastrada.\n");
        printf("Cadastre uma praga antes de registrar uma ocorrencia.\n");
        return;
    }

    //Consulta a quantidade de talhoes existentes antes de permitir o cadastro de uma ocorrencia
    const char *sqlQuantidadeTalhoes =
        "SELECT COUNT(*) FROM talhoes;";

    if (sqlite3_prepare_v2(db, sqlQuantidadeTalhoes, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar talhoes.\n");
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        quantidadeTalhoes = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    //Impede o cadastro de ocorrencias quando nenhum talhao estiver cadastrado
    if (quantidadeTalhoes == 0) {
        printf("\nNenhum talhao cadastrado.\n");
        printf("Cadastre um talhao antes de registrar uma ocorrencia.\n");
        return;
    }

    printf("Digite o codigo da ocorrencia: ");

    if (scanf("%d", &ocorrencia.codigo) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Verifica se o codigo da ocorrencia ja existe antes de continuar o cadastro
    if (buscarOcorrencia(db, ocorrencia.codigo) == 1) {
        printf("\nJa existe uma ocorrencia com este codigo.\n");
        return;
    }

    printf("Digite o codigo da praga: ");

    if (scanf("%d", &ocorrencia.codigoPraga) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Verifica se a praga informada existe antes de vincula-la a ocorrencia
    if (buscarPraga(db, ocorrencia.codigoPraga) == -1) {
        printf("\nPraga nao encontrada.\n");
        return;
    }

    printf("Digite o codigo do talhao: ");

    if (scanf("%d", &ocorrencia.codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Verifica se o talhao informado existe antes de vincula-lo a ocorrencia
    if (buscarTalhao(db, ocorrencia.codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    //Consulta a area total do talhao para validar posteriormente a area afetada pela praga
    const char *sqlArea =
        "SELECT area "
        "FROM talhoes "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sqlArea, -1, &stmtArea, NULL) != SQLITE_OK) {
        printf("Erro ao consultar area do talhao.\n");
        return;
    }

    sqlite3_bind_int(stmtArea, 1, ocorrencia.codigoTalhao);

    if (sqlite3_step(stmtArea) == SQLITE_ROW) {
        areaTalhao = sqlite3_column_double(stmtArea, 0);
    }

    sqlite3_finalize(stmtArea);

    printf("Digite o nivel de infestacao (1 a 5): ");

    if (scanf("%d", &ocorrencia.nivelInfestacao) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');

        return;
    }

    //Valida se o nivel de infestacao esta dentro da escala definida entre 1 e 5
    if (ocorrencia.nivelInfestacao < 1 || ocorrencia.nivelInfestacao > 5) {
        printf("\nNivel de infestacao invalido.\n");
        return;
    }

    printf("Digite a area afetada em hectares: ");

    if (scanf("%f", &ocorrencia.areaAfetada) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Impede o cadastro de uma area afetada com valor negativo
    if (ocorrencia.areaAfetada < 0) {
        printf("\nArea afetada invalida.\n");
        return;
    }

    //Impede que a area afetada seja maior que a area total cadastrada para o talhao
    if (ocorrencia.areaAfetada > areaTalhao) {
        printf("\nArea afetada nao pode ser maior que a area do talhao.\n");
        printf("Area total do talhao: %.2f hectares\n", areaTalhao);
        return;
    }

    getchar();

    printf("Digite a data da ocorrencia (DD/MM/AAAA): ");
    fgets(ocorrencia.data, 20, stdin);

    ocorrencia.data[strcspn(ocorrencia.data, "\n")] = '\0';

    //Utiliza a funcao de validacao de data antes de gravar a ocorrencia
    if (!validarData(ocorrencia.data)) {
        printf("\nData invalida.\n");
        printf("Utilize o formato DD/MM/AAAA.\n");
        return;
    }

    //Prepara o comando SQL para inserir a ocorrencia e seus relacionamentos com a praga e o talhao
    const char *sql =
        "INSERT INTO ocorrencias_pragas "
        "(codigo, codigo_praga, codigo_talhao, nivel_infestacao, area_afetada, data) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar cadastro da ocorrencia.\n");
        return;
    }

    //Associa os dados da ocorrencia aos parametros utilizados pelo comando SQL
    sqlite3_bind_int(stmt, 1, ocorrencia.codigo);
    sqlite3_bind_int(stmt, 2, ocorrencia.codigoPraga);
    sqlite3_bind_int(stmt, 3, ocorrencia.codigoTalhao);
    sqlite3_bind_int(stmt, 4, ocorrencia.nivelInfestacao);
    sqlite3_bind_double(stmt, 5, ocorrencia.areaAfetada);
    sqlite3_bind_text(stmt, 6, ocorrencia.data, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nOcorrencia cadastrada com sucesso!\n");
    }
    else {
        printf("\nErro ao cadastrar ocorrencia.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//Função para listar as ocorrências
void listarOcorrencias(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    //Relaciona as ocorrencias com as tabelas de pragas e talhoes para apresentar os dados completos de cada registro
    const char *sql =
        "SELECT o.codigo, "
        "o.codigo_praga, "
        "p.nome, "
        "o.codigo_talhao, "
        "t.nome, "
        "o.nivel_infestacao, "
        "o.area_afetada, "
        "o.data "
        "FROM ocorrencias_pragas o "
        "INNER JOIN pragas p "
        "ON o.codigo_praga = p.codigo "
        "INNER JOIN talhoes t "
        "ON o.codigo_talhao = t.codigo "
        "ORDER BY o.codigo;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar ocorrencias.\n");
        return;
    }

    printf("\n====================================\n");
    printf("        OCORRENCIAS DE PRAGAS\n");
    printf("====================================\n");

    //Controla se pelo menos uma ocorrencia foi encontrada
    int encontrou = 0;

    //Percorre e apresenta todas as ocorrencias retornadas pela consulta
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        encontrou = 1;

        printf("\n-----------------------------\n");
        printf("Codigo da ocorrencia: %d\n", sqlite3_column_int(stmt, 0));
        printf("Codigo da praga: %d\n", sqlite3_column_int(stmt, 1));
        printf("Praga: %s\n", sqlite3_column_text(stmt, 2));
        printf("Codigo do talhao: %d\n", sqlite3_column_int(stmt, 3));
        printf("Nome do talhao: %s\n", sqlite3_column_text(stmt, 4));
        printf("Nivel de infestacao: %d\n", sqlite3_column_int(stmt, 5));
        printf("Area afetada: %.2f hectares\n", sqlite3_column_double(stmt, 6));
        printf("Data: %s\n", sqlite3_column_text(stmt, 7));
    }

    if (!encontrou) {
        printf("\nNenhuma ocorrencia cadastrada.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}

//Função que busca uma ocorrência
int buscarOcorrencia(sqlite3 *db, int codigo)
{
    sqlite3_stmt *stmt;

    //Consulta a existencia de uma ocorrencia utilizando seu codigo
    const char *sql =
        "SELECT codigo "
        "FROM ocorrencias_pragas "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    //Retorna 1 quando a ocorrencia for encontrada
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);

    //Retorna -1 quando nenhuma ocorrencia possuir o codigo informado
    return -1;
}

//Edita uma ocorrência cadastrada
void editarOcorrencia(sqlite3 *db)
{
    OcorrenciaPraga ocorrencia;

    float areaTalhao = 0;

    sqlite3_stmt *stmt;
    sqlite3_stmt *stmtArea;

    printf("\n====================================\n");
    printf("          EDITAR OCORRENCIA\n");
    printf("====================================\n");

    printf("Digite o codigo da ocorrencia: ");

    if (scanf("%d", &ocorrencia.codigo) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Confirma a existencia da ocorrencia antes de permitir a edicao
    if (buscarOcorrencia(db, ocorrencia.codigo) == -1) {
        printf("\nOcorrencia nao encontrada.\n");
        return;
    }

    printf("Digite o novo codigo da praga: ");

    if (scanf("%d", &ocorrencia.codigoPraga) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Confirma que a nova praga informada existe
    if (buscarPraga(db, ocorrencia.codigoPraga) == -1) {
        printf("\nPraga nao encontrada.\n");
        return;
    }

    printf("Digite o novo codigo do talhao: ");

    if (scanf("%d", &ocorrencia.codigoTalhao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Confirma que o novo talhao informado existe
    if (buscarTalhao(db, ocorrencia.codigoTalhao) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    //Consulta a area do novo talhao para validar a area afetada informada
    const char *sqlArea =
        "SELECT area "
        "FROM talhoes "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sqlArea, -1, &stmtArea, NULL) != SQLITE_OK) {
        printf("Erro ao consultar area do talhao.\n");
        return;
    }

    sqlite3_bind_int(stmtArea, 1, ocorrencia.codigoTalhao);

    if (sqlite3_step(stmtArea) == SQLITE_ROW) {
        areaTalhao = sqlite3_column_double(stmtArea, 0);
    }

    sqlite3_finalize(stmtArea);

    printf("Digite o novo nivel de infestacao (1 a 5): ");

    if (scanf("%d", &ocorrencia.nivelInfestacao) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Valida se o novo nivel de infestacao permanece dentro da escala permitida
    if (ocorrencia.nivelInfestacao < 1 || ocorrencia.nivelInfestacao > 5) {
        printf("\nNivel de infestacao invalido.\n");
        return;
    }

    printf("Digite a nova area afetada em hectares: ");

    if (scanf("%f", &ocorrencia.areaAfetada) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Impede a utilizacao de uma area afetada negativa
    if (ocorrencia.areaAfetada < 0) {
        printf("\nArea afetada invalida.\n");
        return;
    }

    //Impede que a nova area afetada ultrapasse a area total do talhao
    if (ocorrencia.areaAfetada > areaTalhao) {
        printf("\nArea afetada nao pode ser maior que a area do talhao.\n");
        printf("Area total do talhao: %.2f hectares\n", areaTalhao);
        return;
    }

    getchar();

    printf("Digite a nova data (DD/MM/AAAA): ");
    fgets(ocorrencia.data, 20, stdin);

    ocorrencia.data[strcspn(ocorrencia.data, "\n")] = '\0';

    //Valida o formato e os valores da nova data informada
    if (!validarData(ocorrencia.data)) {
        printf("\nData invalida.\n");
        printf("Utilize o formato DD/MM/AAAA.\n");
        return;
    }

    //Prepara o comando SQL que atualiza os dados da ocorrencia selecionada
    const char *sql =
        "UPDATE ocorrencias_pragas "
        "SET codigo_praga = ?, "
        "codigo_talhao = ?, "
        "nivel_infestacao = ?, "
        "area_afetada = ?, "
        "data = ? "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar edicao da ocorrencia.\n");
        return;
    }

    //Associa os novos valores e o codigo da ocorrencia aos parametros do comando UPDATE
    sqlite3_bind_int(stmt, 1, ocorrencia.codigoPraga);
    sqlite3_bind_int(stmt, 2, ocorrencia.codigoTalhao);
    sqlite3_bind_int(stmt, 3, ocorrencia.nivelInfestacao);
    sqlite3_bind_double(stmt, 4, ocorrencia.areaAfetada);
    sqlite3_bind_text(stmt, 5, ocorrencia.data, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, ocorrencia.codigo);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nOcorrencia editada com sucesso!\n");
    }
    else {
        printf("\nErro ao editar ocorrencia.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//Exclui uma ocorrência cadastrada
void excluirOcorrencia(sqlite3 *db)
{
    int codigo;
    char confirmacao;
    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          EXCLUIR OCORRENCIA\n");
    printf("====================================\n");

    printf("Digite o codigo da ocorrencia: ");

    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Confirma que a ocorrencia existe antes de solicitar sua exclusao
    if (buscarOcorrencia(db, codigo) == -1) {
        printf("\nOcorrencia nao encontrada.\n");
        return;
    }

    //Solicita confirmacao do usuario antes de remover definitivamente a ocorrencia
    printf("Deseja realmente excluir esta ocorrencia? (S/N): ");
    scanf(" %c", &confirmacao);

    if (confirmacao != 'S' && confirmacao != 's') {
        printf("Exclusao cancelada.\n");
        return;
    }

    //Prepara o comando para excluir a ocorrencia correspondente ao codigo informado
    const char *sql =
        "DELETE FROM ocorrencias_pragas "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao excluir ocorrencia.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nOcorrencia excluida com sucesso!\n");
    }

    else {
        printf("\nErro ao excluir ocorrencia.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//Edita os dados de uma praga ja cadastrada utilizando seu codigo
void editarPraga(sqlite3 *db)
{
    int codigo;
    int nivelRisco;
    char nome[50];
    char descricao[200];
    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          EDITAR PRAGA\n");
    printf("====================================\n");

    printf("Digite o codigo da praga: ");

    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    getchar();

    //Confirma que a praga existe antes de permitir a alteracao dos seus dados
    if (buscarPraga(db, codigo) == -1) {
        printf("Praga nao encontrada.\n");
        return;
    }

    printf("Digite o novo nome da praga: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("Digite o que a praga faz: ");
    fgets(descricao, sizeof(descricao), stdin);
    descricao[strcspn(descricao, "\n")] = '\0';

    //Repete a leitura ate que seja informado um nivel de risco valido entre 1 e 4
    do {
        printf("Digite o novo nivel de risco (1 a 4): ");

        if (scanf("%d", &nivelRisco) != 1) {
            printf("\nDigite somente numeros.\n");
            while (getchar() != '\n');
            nivelRisco = 0;
            continue;
        }

        if (nivelRisco < 1 || nivelRisco > 4) {
            printf("Nivel de risco invalido.\n");
        }

    } 
    
    while (nivelRisco < 1 || nivelRisco > 4);

    //Prepara o comando SQL para atualizar nome, descricao e nivel de risco da praga
    const char *sql =
        "UPDATE pragas "
        "SET nome = ?, descricao = ?, nivel_risco = ? "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar edicao da praga.\n");
        return;
    }

    //Associa os novos dados e o codigo da praga aos parametros do comando UPDATE
    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, descricao, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, nivelRisco);
    sqlite3_bind_int(stmt, 4, codigo);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Praga editada com sucesso.\n");
    } else {
        printf("Erro ao editar praga.\n");
    }

    sqlite3_finalize(stmt);
}

//Exclui uma praga cadastrada utilizando seu codigo
void excluirPraga(sqlite3 *db)
{
    int codigo;
    char confirmacao;
    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          EXCLUIR PRAGA\n");
    printf("====================================\n");

    printf("Digite o codigo da praga: ");

    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");
        while (getchar() != '\n');
        return;
    }

    //Confirma que a praga existe antes de solicitar sua exclusao
    if (buscarPraga(db, codigo) == -1) {
        printf("Praga nao encontrada.\n");
        return;
    }

    //Solicita confirmacao do usuario antes de excluir definitivamente a praga
    printf("Deseja realmente excluir esta praga? (S/N): ");
    scanf(" %c", &confirmacao);

    if (confirmacao != 'S' && confirmacao != 's') {
        printf("Exclusao cancelada.\n");
        return;
    }

    //Prepara o comando SQL para excluir a praga correspondente ao codigo informado
    const char *sql =
        "DELETE FROM pragas "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar exclusao da praga.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    //A exclusao pode ser impedida caso existam ocorrencias vinculadas a praga por meio da chave estrangeira
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Praga excluida com sucesso.\n");
    } else {
        printf("Nao foi possivel excluir a praga.\n");
        printf("Verifique se existem ocorrencias vinculadas a ela.\n");
    }

    sqlite3_finalize(stmt);
}