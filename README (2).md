# Modulo de Monitoramento Climatico e Relatorios

Parte do Sistema de Gestao Agricola referente ao monitoramento climatico
e aos relatorios do sistema. Integra com o modulo de Talhoes (`talhoes.h`/`talhoes.c`)
ja existente no projeto do grupo.

## Arquivos

| Arquivo | Descricao |
|---|---|
| `clima.h` / `clima.c` | Cadastro e consulta dos dados climaticos (temperatura, umidade, data e hora) por talhao. |
| `relatorios.h` / `relatorios.c` | Geracao de relatorios climaticos, cruzamento com pragas, exportacao para TXT/CSV e resumo por periodo. |
| `main.c` | Main do sistema com os menus 3 (Monitoramento Climatico) e 6 (Relatorios) ja integrados. Os demais modulos permanecem "em desenvolvimento". |

Dependencias esperadas no mesmo diretorio: `talhoes.h` e `talhoes.c` (modulo de talhoes do grupo).

## Como compilar

```
gcc -Wall -o sistema main.c talhoes.c clima.c relatorios.c
./sistema
```

## Funcionalidades

### Monitoramento Climatico (`clima.c`)

- **Registrar dados climaticos**: pede o codigo do talhao (valida se ele existe,
  usando `buscarTalhao` do modulo de talhoes), temperatura, umidade, data (`dd/mm/aaaa`)
  e hora (`hh:mm`).
- **Listar historico climatico**: mostra todos os registros ja cadastrados.
- **Ver serie historica por talhao**: filtra e mostra apenas os registros de um
  talhao especifico.

Os registros ficam em um vetor `RegistroClima registrosClima[MAX_CLIMA]`
(`MAX_CLIMA = 500`), guardado em memoria durante a execucao do programa
(nao persiste em arquivo entre execucoes).

### Relatorios (`relatorios.c`)

- **Relatorio climatico geral**: lista todos os registros de clima com o nome
  do talhao correspondente.
- **Relatorio cruzado (pragas x clima)**: cruza os dados de pragas com os
  registros climaticos do mesmo talhao. Veja a observacao sobre `PragaResumo` abaixo.
- **Exportar para TXT**: gera o arquivo `relatorio_climatico.txt` no diretorio
  onde o programa foi executado.
- **Exportar para CSV**: gera o arquivo `relatorio_climatico.csv`, com cabecalho
  `Codigo Talhao,Nome Talhao,Data,Hora,Temperatura,Umidade`, pronto pra abrir no Excel.
- **Resumo por periodo**: pede mes e ano, calcula media de temperatura, media de
  umidade, temperatura maxima e minima dos registros daquele periodo.

## Observacao importante: integracao com o modulo de Pragas

O modulo de pragas do grupo ainda nao estava pronto quando esse codigo foi
escrito. Para o relatorio cruzado funcionar e compilar, foi criada uma struct
provisoria em `relatorios.h`:

```c
typedef struct {
    int codigoTalhao;
    char nomePraga[50];
    int nivelInfestacao;
} PragaResumo;
```

Quando o `pragas.h` real do colega estiver pronto, e preciso:

1. Trocar `PragaResumo` pela struct real de Praga (ou ajustar os nomes de
   campo usados dentro de `gerarRelatorioPragasClima`, em `relatorios.c`).
2. Trocar o vetor `PragaResumo pragas[MAX_PRAGAS_RESUMO]` criado em `main.c`
   pelo vetor real de pragas do outro modulo.

## Testes realizados

O fluxo completo foi compilado com `gcc -Wall` (sem warnings) e testado
manualmente: cadastro de talhao, registro de clima, listagem, serie historica,
relatorio geral, exportacao TXT/CSV e resumo por periodo — tudo funcionando
sem travar.
