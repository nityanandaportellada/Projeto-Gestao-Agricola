#ifndef CLIMA_H
#define CLIMA_H

#include <sqlite3.h>

typedef struct {
    int codigoTalhao;
    float temperatura;
    float umidade;
    char data[12];
    char hora[7];
} RegistroClima;

void registrarClima(sqlite3 *db);
void listarHistoricoClima(sqlite3 *db);
void visualizarSerieTalhao(sqlite3 *db);
void editarClima(sqlite3 *db);
void excluirClima(sqlite3 *db);
int calcularRiscoTemperatura(float temperatura);
int calcularRiscoUmidade(float umidade);
void calcularRiscosClimaticos(float temperatura, float umidade, int *riscoTemperatura, int *riscoUmidade);
int validarData(char data[]);
int validarHora(char hora[]);

#endif