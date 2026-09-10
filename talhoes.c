//Modulo responsavel pelo cadastro, consulta, edicao e exclusao dos talhoes da fazenda
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#include "talhoes.h"


//Cadastra o talhao (unidade basica da fazenda)

void cadastrarTalhao(sqlite3 *db) {
    int codigo;
    char nome[50];
    float area;
    char plantacao[50];
    char localizacao[100];

    sqlite3_stmt *stmt;

    //Define o comando SQL utilizado para inserir um novo talhao no banco de dados
    const char *sql =
        "INSERT INTO talhoes "
        "(codigo, nome, area, plantacao, localizacao) "
        "VALUES (?, ?, ?, ?, ?);";

    printf("\n====================================\n");
    printf("          CADASTRO DE TALHAO\n");
    printf("====================================\n");

    printf("Digite o codigo do talhao: ");

    //Valida se o codigo informado pelo usuario e um numero inteiro
    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Verifica se há um talhao com este codigo cadastrado

    const char *sqlBusca =
        "SELECT codigo FROM talhoes WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sqlBusca, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar talhoes.\n");
        return;
    }

    //Associa o codigo informado ao parametro utilizado na consulta
    sqlite3_bind_int(stmt, 1, codigo);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("\nERRO: Ja existe um talhao com este codigo!\n");
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    getchar();

    printf("Digite o nome do talhao: ");
    fgets(nome, 50, stdin);

    //Remove a quebra de linha armazenada pelo fgets
    nome[strcspn(nome, "\n")] = '\0';

    printf("Digite a area do talhao em hectares: ");

    if (scanf("%f", &area) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Impede o cadastro de talhoes com area igual ou inferior a zero
    if (area <= 0) {
        printf("\nArea do talhao invalida.\n");
        printf("A area deve ser maior que zero.\n");
        return;
    }

    getchar();

    printf("Digite o tipo de plantacao: ");
    fgets(plantacao, 50, stdin);

    plantacao[strcspn(plantacao, "\n")] = '\0';

    printf("Digite a localizacao do talhao: ");
    fgets(localizacao, 100, stdin);

    localizacao[strcspn(localizacao, "\n")] = '\0';

    //Prepara para inserir no banco

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar cadastro.\n");
        return;
    }
   
    //Associa os dados informados aos parametros do comando INSERT
    sqlite3_bind_int(stmt, 1, codigo);
    sqlite3_bind_text(stmt, 2, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, area);
    sqlite3_bind_text(stmt, 4, plantacao, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, localizacao, -1, SQLITE_TRANSIENT);

    //Executa o inserte para conclusão da insersão dos dados no banco
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nTalhao cadastrado com sucesso!\n");
    } 
    
    else {
        printf("\nErro ao cadastrar o talhao, tente novamente.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    //Libera o comando preparado depois da execucao
    sqlite3_finalize(stmt);
}


//função para listar os talhoes cadastrados
void listarTalhoes(sqlite3 *db) {
    sqlite3_stmt *stmt;

    //Seleciona todos os talhoes cadastrados e organiza os resultados pelo codigo
    const char *sql =
        "SELECT codigo, nome, area, plantacao, localizacao "
        "FROM talhoes "
        "ORDER BY codigo;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao consultar talhoes.\n");
        return;
    }

    printf("\n====================================\n");
    printf("          TALHOES CADASTRADOS\n");
    printf("====================================\n");

    //Controla se pelo menos um talhao foi encontrado durante a consulta
    int encontrou = 0;

    //Percorre todos os registros retornados pelo banco
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        encontrou = 1;

        printf("\n-----------------------------\n");
        printf("Codigo: %d\n", sqlite3_column_int(stmt, 0));
        printf("Nome: %s\n", sqlite3_column_text(stmt, 1));
        printf("Area: %.2f hectares\n", sqlite3_column_double(stmt, 2));
        printf("Plantacao: %s\n", sqlite3_column_text(stmt, 3));
        printf("Localizacao: %s\n", sqlite3_column_text(stmt, 4));
    }

    if (!encontrou) {
        printf("\nNenhum talhao cadastrado.\n");
    }

    printf("-----------------------------\n");

    sqlite3_finalize(stmt);
}

//Bunca um talhao cadastrado
int buscarTalhao(sqlite3 *db, int codigo) {
    sqlite3_stmt *stmt;

    //Consulta se existe um talhao com o codigo recebido pela funcao
    const char *sql =
        "SELECT codigo "
        "FROM talhoes "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    //Retorna 1 quando o talhao for encontrado
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);

    //Retorna -1 quando nenhum talhao com o codigo informado for encontrado
    return -1;
}

//Edita um talhao ja cadastrado
void editarTalhao(sqlite3 *db) {
    int codigo;
    int codigoEncontrado;

    char nome[50];
    float area;
    char plantacao[50];
    char localizacao[100];

    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("             EDITAR TALHAO\n");
    printf("====================================\n");

    printf("Digite o codigo do talhao: ");

    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Verifica se o talhao informado existe antes de continuar com a edicao
    codigoEncontrado = buscarTalhao(db, codigo);

    if (codigoEncontrado == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    getchar();

    printf("Digite o novo nome: ");
    fgets(nome, 50, stdin);

    nome[strcspn(nome, "\n")] = '\0';

    printf("Digite a nova area: ");

    if (scanf("%f", &area) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    if (area <= 0) {
        printf("\nArea do talhao invalida.\n");
        printf("A area deve ser maior que zero.\n");
        return;
    }

    getchar();

    printf("Digite o novo tipo de plantacao: ");
    fgets(plantacao, 50, stdin);

    plantacao[strcspn(plantacao, "\n")] = '\0';

    printf("Digite a nova localizacao: ");
    fgets(localizacao, 100, stdin);

    localizacao[strcspn(localizacao, "\n")] = '\0';

    //Define o comando SQL responsavel por atualizar os dados do talhao selecionado
    const char *sql =
        "UPDATE talhoes "
        "SET nome = ?, "
        "area = ?, "
        "plantacao = ?, "
        "localizacao = ? "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {

        printf("Erro ao preparar alteracao.\n");
        return;
    }

    //Associa os novos dados e o codigo do talhao aos parametros do comando UPDATE
    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, area);
    sqlite3_bind_text(stmt, 3, plantacao, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, localizacao, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, codigo);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nTalhao alterado com sucesso!\n");

    } 
    else {
        printf("\nErro ao alterar talhao.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

//exclui um talhao cadastrado
void excluirTalhao(sqlite3 *db)
{
    int codigo;
    char confirmacao;

    sqlite3_stmt *stmt;

    printf("\n====================================\n");
    printf("          EXCLUIR TALHAO\n");
    printf("====================================\n");

    printf("Digite o codigo do talhao: ");

    if (scanf("%d", &codigo) != 1) {
        printf("\nDigite somente numeros.\n");

        while (getchar() != '\n');
        return;
    }

    //Confirma se o talhao existe antes de solicitar sua exclusao
    if (buscarTalhao(db, codigo) == -1) {
        printf("\nTalhao nao encontrado.\n");
        return;
    }

    //Solicita confirmacao para evitar a exclusao acidental de um talhao
    printf("Deseja realmente excluir este talhao? (S/N): ");
    scanf(" %c", &confirmacao);

    if (confirmacao != 'S' && confirmacao != 's') {
        printf("Exclusao cancelada.\n");
        return;
    }

    //Define o comando SQL responsavel por excluir o talhao selecionado
    const char *sql =
        "DELETE FROM talhoes "
        "WHERE codigo = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar exclusao do talhao.\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, codigo);

    //A exclusao pode ser impedida quando existirem ocorrencias ou registros climaticos relacionados ao talhao
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("\nTalhao excluido com sucesso!\n");
    }
    else {
        printf("\nNao foi possivel excluir o talhao.\n");
        printf("Verifique se existem ocorrencias ou registros climaticos associados.\n");
        printf("Mensagem: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}